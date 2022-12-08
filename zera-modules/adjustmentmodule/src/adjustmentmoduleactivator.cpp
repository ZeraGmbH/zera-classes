#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"
#include "tasktimeoutdecorator.h"
#include "taskparallel.h"
#include "taskserverconnectionstart.h"
#include "taskrmsendident.h"
#include "taskrmcheckresourcetype.h"
#include "taskrmcheckchannelsavail.h"
#include "taskreadchannelipport.h"
#include "taskchannelpcbconnectionsstart.h"
#include "taskrmreadchannelalias.h"

#include "errormessages.h"
#include "reply.h"
#include "signalwaiter.h"

namespace ADJUSTMENTMODULE
{

AdjustmentModuleActivator::AdjustmentModuleActivator(cAdjustmentModule *module,
                                                     Zera::Proxy::cProxy *proxy,
                                                     std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                                                     AdjustmentModuleActivateDataPtr activationData) :
    m_activationData(activationData),
    m_module(module),
    m_proxy(proxy),
    m_configuration(pConfiguration)
{
}

void AdjustmentModuleActivator::activate()
{
    if(!checkExternalVeinComponents())
        return;
    openRMConnection();

    m_activationTasks.appendTask(TaskServerConnectionStart::create(m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.appendTask(TaskRmSendIdent::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.appendTask(TaskRmCheckResourceType::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.appendTask(TaskRmCheckChannelsAvail::create(m_rmInterface, getConfData()->m_AdjChannelList,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));

    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(int currChannel = 0; currChannel<getConfData()->m_nAdjustmentChannelCount; currChannel++) {
        QString channelName = getConfData()->m_AdjChannelList.at(currChannel); // current channel m0/m1/
        parallelTasks->addTask(TaskReadChannelIpPort::create(m_rmInterface, channelName, m_activationData->m_chnPortHash,
                                                             TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceInfoErrMsg); }));
    }
    m_activationTasks.appendTask(std::move(parallelTasks));
    m_activationTasks.appendTask(TaskChannelPcbConnectionsStart::create(m_activationData,
                                                                        getConfData()->m_AdjChannelList,
                                                                        getConfData()->m_PCBSocket.m_sIP,
                                                                        CONNECTION_TIMEOUT));
    parallelTasks = TaskParallel::create();
    for(int currChannel = 0; currChannel<getConfData()->m_nAdjustmentChannelCount; currChannel++) {
        QString channelName = getConfData()->m_AdjChannelList.at(currChannel); // current channel m0/m1/
        parallelTasks->addTask(TaskRmReadChannelAlias::create(m_activationData, channelName,
                                                              TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
    }
    m_activationTasks.appendTask(std::move(parallelTasks));

    connect(&m_activationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::activateContinue);
    m_activationTasks.start();
}

void AdjustmentModuleActivator::activateContinue(bool ok)
{
    if(!ok)
        return;
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
    for(m_currentChannel = 0; m_currentChannel<getConfData()->m_nAdjustmentChannelCount; m_currentChannel++) {
        QString channelName = getConfData()->m_AdjChannelList.at(m_currentChannel); // current channel m0/m1/
        if(!regNotifier(channelName)->wait())
            return;
        if(!readRangeList(channelName)->wait())
            return;
    }
    m_bActive = true;
    emit sigActivationReady();
}

void AdjustmentModuleActivator::deactivate()
{
    for(int pcbInterfaceNo = 0; pcbInterfaceNo<m_activationData->m_pcbInterfaceList.count(); ++pcbInterfaceNo) {
        if(!unregNotifier(pcbInterfaceNo)->wait())
            return;
    }
    m_bActive = false;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::setupServerResponseHandlers()
{
    setUpRegisterNotifierHandler();
    setUpRangeListHandler();

    setUpUnregisterNotifierHandler();
}

bool AdjustmentModuleActivator::checkExternalVeinComponents()
{
    bool ok = true;
    adjInfoType adjInfo = getConfData()->m_ReferenceAngle;
    if (!m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        ok = false;
    adjInfo = getConfData()->m_ReferenceFrequency;
    if (!m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
        ok = false;

    for (int i = 0; ok && i<getConfData()->m_nAdjustmentChannelCount; i++) {
        // we test if all configured actual value data exist
        QString chn = getConfData()->m_AdjChannelList.at(i);

        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->amplitudeAdjInfo;
        if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            ok = false;
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->phaseAdjInfo;
        if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            ok = false;
        adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->offsetAdjInfo;
        if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            ok = false;
    }
    return ok;
}

void AdjustmentModuleActivator::openRMConnection()
{
    m_rmInterface = std::make_shared<Zera::Server::cRMInterface>();
    m_rmClient = m_proxy->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_rmInterface->setClientSmart(m_rmClient);
}


BlockedWaitInterfacePtr AdjustmentModuleActivator::regNotifier(QString channelName)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::activationContinue,
                                                                    this, &AdjustmentModuleActivator::activationError,
                                                                    TRANSACTION_TIMEOUT);
    Zera::Server::cPCBInterface* pcbInterface = m_activationData->m_adjustChannelInfoHash[channelName]->m_pPCBInterface;
    m_MsgNrCmdList[pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(channelName),"1")] = registerNotifier;
    return waiter;
}

void AdjustmentModuleActivator::setUpRegisterNotifierHandler()
{
    m_cmdFinishCallbacks[registerNotifier] = [&](quint8 reply, QVariant) {
        if (reply == ack)
            emit activationContinue();
        else
            notifyActivationError(tr(registerpcbnotifierErrMsg));
    };
}

BlockedWaitInterfacePtr AdjustmentModuleActivator::readRangeList(QString channelName)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::activationContinue,
                                                                    this, &AdjustmentModuleActivator::activationError,
                                                                    TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_activationData->m_adjustChannelInfoHash[channelName]->m_pPCBInterface->getRangeList(channelName)] = readrangelist;
    return waiter;
}

void AdjustmentModuleActivator::setUpRangeListHandler()
{
    m_cmdFinishCallbacks[readrangelist] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            m_activationData->m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(m_currentChannel)]->m_sRangelist = answer.toStringList();
            emit activationContinue();
        }
        else
            notifyActivationError(tr(readrangelistErrMsg));
    };
}

BlockedWaitInterfacePtr AdjustmentModuleActivator::unregNotifier(int pcbInterfaceNo)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::deactivationContinue,
                                                                    this, &AdjustmentModuleActivator::deactivationError,
                                                                    TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_activationData->m_pcbInterfaceList[pcbInterfaceNo]->unregisterNotifiers() ] = unregisterNotifiers ;
    return waiter;
}

void AdjustmentModuleActivator::setUpUnregisterNotifierHandler()
{
    m_cmdFinishCallbacks[unregisterNotifiers] = [&](quint8 reply, QVariant) {
        if (reply == ack)
            emit deactivationContinue();
        else
            notifyActivationError(tr(unregisterpcbnotifierErrMsg));
    };
}

void AdjustmentModuleActivator::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (m_MsgNrCmdList.contains(msgnr)) {
        handleFinishCallback(m_MsgNrCmdList.take(msgnr), reply, answer);
    }
}

cAdjustmentModuleConfigData *AdjustmentModuleActivator::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_configuration.get())->getConfigurationData();
}

}
