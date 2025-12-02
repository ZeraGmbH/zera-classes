#include "hotplugcontrolsmodulecontroller.h"
#include "rpcactivatepushbutton.h"
#include "rpcreadlockstate.h"
#include "servicechannelnamehelper.h"
#include <taskemobreadexchangedata.h>
#include <taskemobwriteexchangedata.h>
#include <errormessages.h>
#include <intvalidator.h>
#include <reply.h>
#include <scpi.h>
#include <stringvalidator.h>
#include <QJsonDocument>

namespace HOTPLUGCONTROLSMODULE {

static int constexpr statusEntityId = 1150;

HotplugControlsModuleController::HotplugControlsModuleController(cHotplugControlsModule *module) :
    cModuleActivist(module->getVeinModuleName()),
    m_module(module),
    m_pcbConnection(m_module->getNetworkConfig())
{
    connect(&m_activationTasks, &TaskTemplate::sigFinish, this, &HotplugControlsModuleController::onActivateDone);
}

void HotplugControlsModuleController::activate()
{
    m_activationTasks.addSub(m_pcbConnection.createConnectionTask());
    m_activationTasks.start();
}

void HotplugControlsModuleController::deactivate()
{
    emit m_module->deactivationContinue();
}

void HotplugControlsModuleController::generateVeinInterface()
{
    QString key;
    VfRpcEventSystemSimplified *rpcEventSystem = m_module->getRpcEventSystem();
    m_observer = m_module->getSharedChannelRangeObserver();

    std::shared_ptr<RpcActivatePushButton> rpcEmobActivatePushButton = std::make_shared<RpcActivatePushButton>(m_pcbConnection.getInterface(),
                                                                                                               m_observer,
                                                                                                               rpcEventSystem,
                                                                                                               m_module->getEntityId());
    m_pEmobPushButtonRpc = std::make_shared<VfModuleRpc>(rpcEmobActivatePushButton,
                                                         "Press EMOB-pushbutton");
    m_pEmobPushButtonRpc->setRPCScpiInfo("EMOB",
                                        QString("PBPRESS"),
                                        SCPI::isCmdwP,
                                        rpcEmobActivatePushButton->getSignature());
    m_pEmobPushButtonRpc->setValidator(new cStringValidator(QString("IL1;IL2;IL3;IAUX")));
    m_pEmobPushButtonRpc->canAcceptOptionalParam();
    m_module->m_veinModuleRPCMap[rpcEmobActivatePushButton->getSignature()] = m_pEmobPushButtonRpc; // for modules use

    std::shared_ptr<RPCReadLockState> rpcEmobReadLockState = std::make_shared<RPCReadLockState>(m_pcbConnection.getInterface(),
                                                                                                m_observer,
                                                                                                rpcEventSystem,
                                                                                                m_module->getEntityId());
    m_pEmobLockStateRpc = std::make_shared<VfModuleRpc>(rpcEmobReadLockState,
                                                        "EMOB plug lock state: 0:unknown 1:open 2:locking 3:locked 4:error");
    m_pEmobLockStateRpc->setRPCScpiInfo("EMOB",
                                        QString("EMLOCKSTATE"),
                                        SCPI::isQuery,
                                        rpcEmobReadLockState->getSignature());
    m_pEmobLockStateRpc->setValidator(new cStringValidator(QString("IL1;IL2;IL3;IAUX")));
    m_pEmobLockStateRpc->canAcceptOptionalParam();
    m_module->m_veinModuleRPCMap[rpcEmobReadLockState->getSignature()] = m_pEmobLockStateRpc; // for modules use

    m_pControllersFound = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                                key = QString("ACT_ControllersFound"),
                                                QString("Detect if Emob and Mt650e are connected"),
                                                QVariant());
    m_module->m_veinModuleParameterMap[key] = m_pControllersFound;

    VeinStorage::StorageComponentPtr value = m_module->getStorageDb()->findComponent(statusEntityId, "INF_Instrument");
    if(value) {
        connect(value.get(), &VeinStorage::AbstractComponent::sigValueChange, this, &HotplugControlsModuleController::controllersFound);
        //On startup, we miss the slot. So call it in case the device starts with hotplugs connected
        controllersFound(value->getValue());
    }
}

void HotplugControlsModuleController::onActivateDone(bool ok)
{
    if(ok) {
        connect(m_pcbConnection.getInterface().get(), &AbstractServerInterface::serverAnswer,
                this, &HotplugControlsModuleController::catchInterfaceAnswer);
        emit m_module->activationContinue();
    }
}

void HotplugControlsModuleController::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) {
        // 0 was reserved for async. messages
    }
    else {
        if (m_MsgNrCmdList.contains(msgnr)) {
            int cmd = m_MsgNrCmdList.take(msgnr);
            switch (cmd)
            {
            case activatepushbutton:
                if (reply != ack)
                    notifyError(pushbuttonErrMsg);
                break;
            }
        }
    }
}

void HotplugControlsModuleController::controllersFound(QVariant value)
{
    bool emobFound = false;
    bool mt650eFound = false;
    QString emobChannelAlias = "";
    QString mt650eChannelAlias = "";
    QJsonDocument doc = QJsonDocument::fromJson(value.toString().toUtf8());
    QJsonObject instrumentsObj = doc.object();
    if(!instrumentsObj.isEmpty()) {
        for(auto it = instrumentsObj.begin(); it != instrumentsObj.end(); ++it) {
            QString key = it.key();
            QString value = instrumentsObj[key].toString();
            if(value.contains("Emob", Qt::CaseInsensitive)) {
                emobChannelAlias = key;
                emobFound = true;
            }
            if(value.contains("Mt650e", Qt::CaseInsensitive)) {
                mt650eChannelAlias = key;
                mt650eFound = true;
            }
        }
    }
    if(emobFound && mt650eFound)
        m_pControllersFound->setValue(true);
    else
        m_pControllersFound->setValue(false);
    readWriteData(emobChannelAlias, mt650eChannelAlias);
}

void HotplugControlsModuleController::readWriteData(QString emobChannelAlias, QString mt650eChannelAlias)
{
    int timeout = 50;
    int emobId = getEmobId(emobChannelAlias);
    QString emobChannelMName = ServiceChannelNameHelper::getChannelMName(emobChannelAlias, m_observer);
    QString mt650eChannelMName = ServiceChannelNameHelper::getChannelMName(mt650eChannelAlias, m_observer);

    if(m_pControllersFound->getValue().toBool()) {
        TaskContainerInterfacePtr tasks = TaskContainerSequence::create();
        std::shared_ptr<QByteArray> dataReceived = std::make_shared<QByteArray>();
        TaskTemplatePtr readTtask = TaskEmobReadExchangeData::create(m_pcbConnection.getInterface(),
                                                                     dataReceived, mt650eChannelMName,
                                                                     timeout);
        TaskTemplatePtr writeTtask = TaskEmobWriteExchangeData::create(m_pcbConnection.getInterface(),
                                                                       emobChannelMName, emobId, dataReceived,
                                                                       timeout);
        tasks->addSub(std::move(readTtask));
        tasks->addSub(std::move(writeTtask));
        tasks->start();
    }
    else {
        if(!emobChannelMName.isEmpty()) {
            TaskTemplatePtr writeTtask = TaskEmobWriteExchangeData::create(m_pcbConnection.getInterface(),
                                                                           emobChannelMName, emobId, std::make_shared<QByteArray>(),
                                                                           timeout);
            writeTtask->start();
        }
    }
}

int HotplugControlsModuleController::getEmobId(QString emobChannelAlias)
{
    if(emobChannelAlias == "IL1")
        return 0;
    else if(emobChannelAlias == "IL2")
        return 1;
    else if(emobChannelAlias == "IL2")
        return 2;
    else
        return 3;
}

}
