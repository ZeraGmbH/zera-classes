#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"
#include "taskserverconnectionstart.h"
#include "taskrmsendident.h"
#include "taskrmcheckresourcetype.h"
#include "taskrmcheckchannelsavail.h"
#include "taskreadchannelipport.h"
#include "taskchannelpcbconnectionsstart.h"
#include "taskrmreadchannelalias.h"
#include "taskchannelregisternotifier.h"
#include "taskchannelreadranges.h"
#include "taskchannelunregisternotifier.h"
#include "errormessages.h"

namespace ADJUSTMENTMODULE
{

AdjustmentModuleActivator::AdjustmentModuleActivator(std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                                                     AdjustmentModuleActivateDataPtr activationData) :
    m_activationData(activationData),
    m_configuration(pConfiguration)
{
}

TaskCompositePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskParallelPtr channelReadTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        TaskSequencePtr perChannelTasks = TaskSequence::create();
        perChannelTasks->addSubTask(TaskRmReadChannelAlias::create(m_activationData, channelName,
                                                                   TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->addSubTask(TaskChannelRegisterNotifier::create(m_activationData, channelName,
                                                                        TRANSACTION_TIMEOUT, [&]{ emit errMsg(registerpcbnotifierErrMsg); }));
        perChannelTasks->addSubTask(TaskChannelReadRanges::create(m_activationData, channelName,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        channelReadTasks->addSubTask(std::move(perChannelTasks));
    }
    return channelReadTasks;
}

void AdjustmentModuleActivator::activate()
{
    openRMConnection();

    m_activationTasks.addSubTask(TaskServerConnectionStart::create(m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.addSubTask(TaskRmSendIdent::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.addSubTask(TaskRmCheckResourceType::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.addSubTask(TaskRmCheckChannelsAvail::create(m_rmInterface, getConfData()->m_AdjChannelList,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));

    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        parallelTasks->addSubTask(TaskReadChannelIpPort::create(m_rmInterface, channelName, m_activationData->m_chnPortHash,
                                                             TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceInfoErrMsg); }));
    }
    m_activationTasks.addSubTask(std::move(parallelTasks));
    m_activationTasks.addSubTask(TaskChannelPcbConnectionsStart::create(m_activationData,
                                                                        getConfData()->m_AdjChannelList,
                                                                        getConfData()->m_PCBSocket.m_sIP,
                                                                        CONNECTION_TIMEOUT));
    m_activationTasks.addSubTask(getChannelsReadTasks());

    connect(&m_activationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::activateContinue);
    m_activationTasks.start();
}

void AdjustmentModuleActivator::activateContinue(bool ok)
{
    if(!ok)
        return;
    emit sigActivationReady();
}

void AdjustmentModuleActivator::deactivate()
{
    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        parallelTasks->addSubTask(TaskChannelUnregisterNotifier::create(m_activationData, channelName,
                                                                     TRANSACTION_TIMEOUT, [&]{ emit errMsg(unregisterpcbnotifierErrMsg); }));
    }
    m_deactivationTasks.addSubTask(std::move(parallelTasks));
    connect(&m_deactivationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::deactivateContinue);
    m_deactivationTasks.start();
}

void AdjustmentModuleActivator::deactivateContinue(bool ok)
{
    if(!ok)
        return;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::openRMConnection()
{
    m_rmInterface = std::make_shared<Zera::Server::cRMInterface>();
    m_rmClient = Zera::Proxy::cProxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_rmInterface->setClientSmart(m_rmClient);
}

cAdjustmentModuleConfigData *AdjustmentModuleActivator::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_configuration.get())->getConfigurationData();
}

}
