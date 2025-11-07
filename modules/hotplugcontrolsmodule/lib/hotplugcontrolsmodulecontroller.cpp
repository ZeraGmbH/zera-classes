#include "hotplugcontrolsmodulecontroller.h"
#include "rpcreadlockstate.h"
#include <errormessages.h>
#include <intvalidator.h>
#include <reply.h>
#include <scpi.h>

namespace HOTPLUGCONTROLSMODULE {


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

    m_pPressPushButton = new VfModuleParameter(m_module->getEntityId(), m_module->getValidatorEventSystem(),
                                               key = QString("PAR_EmobPushButton"),
                                               QString("Activate EMOB-pushbutton (push=1)"),
                                               QVariant((int)0));
    m_pPressPushButton->setScpiInfo("EMOB", QString("PBPRESS"), SCPI::isCmdwP, m_pPressPushButton->getName());
    m_pPressPushButton->setValidator(new cIntValidator(0, 1, 1));
    m_module->m_veinModuleParameterMap[key] = m_pPressPushButton; // for modules use

    VfRpcEventSystemSimplified *rpcEventSystem = m_module->getRpcEventSystem();

    std::shared_ptr<RPCReadLockState> rpcEmobReadLockState = std::make_shared<RPCReadLockState>(m_pcbConnection.getInterface(), rpcEventSystem, m_module->getEntityId());
    rpcEventSystem->addRpc(rpcEmobReadLockState);
    m_pEmobLockStateRpc = std::make_shared<VfModuleRpc>(rpcEmobReadLockState,
                                                        "EMOB plug lock state: 0:unknown 1:open 2:locking 3:locked 4:error");
    m_pEmobLockStateRpc->setRPCScpiInfo("EMOB",
                                        QString("EMLOCKSTATE"),
                                        SCPI::isQuery,
                                        rpcEmobReadLockState->getSignature());
    m_module->m_veinModuleRPCMap[rpcEmobReadLockState->getSignature()] = m_pEmobLockStateRpc; // for modules use
}

void HotplugControlsModuleController::onActivateDone(bool ok)
{
    if(ok) {
        connect(m_pcbConnection.getInterface().get(), &AbstractServerInterface::serverAnswer,
                this, &HotplugControlsModuleController::catchInterfaceAnswer);
        connect(m_pPressPushButton, &VfModuleParameter::sigValueChanged, this, &HotplugControlsModuleController::newPushButton);
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

void HotplugControlsModuleController::newPushButton(QVariant pushbutton)
{
    bool ok;
    int pb = pushbutton.toInt(&ok);
    if (!ok) {
        qWarning("newPushButton NOT ok");
        return;
    }
    if (pb == 1) {
        m_MsgNrCmdList[m_pcbConnection.getInterface()->activatePushButton()] = activatepushbutton;       // send actication via I2C to Emob
        m_pPressPushButton->setValue(0);
    }
    else if (pb == 0) {
        qWarning("newPushButton: 0");
    }
}

}
