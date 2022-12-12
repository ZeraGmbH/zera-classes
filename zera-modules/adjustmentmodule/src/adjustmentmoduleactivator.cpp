#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"
#include "taskserverconnectionstart.h"
#include "taskrmsendident.h"
#include "taskrmcheckresourcetype.h"
#include "taskrmcheckchannelsavail.h"
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
    connect(&m_activationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::onActivateContinue);
    connect(&m_deactivationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::onDeactivateContinue);
    connect(&m_reloadRangesTasks, &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onReloadRanges);
}

TaskCompositePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskParallelPtr channelReadTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        TaskSequencePtr perChannelTasks = TaskSequence::create();
        perChannelTasks->addSubTask(TaskRmReadChannelAlias::create(m_activationData, channelName,
                                                                   TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->addSubTask(TaskChannelReadRanges::create(m_activationData, channelName,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        channelReadTasks->addSubTask(std::move(perChannelTasks));
    }
    return channelReadTasks;
}

void AdjustmentModuleActivator::activate()
{
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        m_activationData->m_adjustChannelInfoHash[channelName] = std::make_unique<cAdjustChannelInfo>();
    }
    openRMConnection();
    openPcbConnection();

    m_activationTasks.addSubTask(TaskServerConnectionStart::create(m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.addSubTask(TaskRmSendIdent::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.addSubTask(TaskRmCheckResourceType::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.addSubTask(TaskRmCheckChannelsAvail::create(m_rmInterface, getConfData()->m_AdjChannelList,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));
    m_activationTasks.addSubTask(TaskServerConnectionStart::create(m_activationData->m_pcbClient, CONNECTION_TIMEOUT));

    m_activationTasks.addSubTask(getChannelsReadTasks());
    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        parallelTasks->addSubTask(TaskChannelRegisterNotifier::create(m_activationData, channelName,
                                                                      TRANSACTION_TIMEOUT, [&]{ emit errMsg(registerpcbnotifierErrMsg); }));
    }
    m_activationTasks.addSubTask(std::move(parallelTasks));
    m_activationTasks.start();
}

void AdjustmentModuleActivator::onActivateContinue(bool ok)
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
    m_deactivationTasks.start();
}

void AdjustmentModuleActivator::reloadRanges()
{
    m_reloadRangesTasks.addSubTask(getChannelsReadTasks());
    m_reloadRangesTasks.start();
}

void AdjustmentModuleActivator::onDeactivateContinue(bool ok)
{
    if(!ok)
        return;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::onReloadRanges(bool ok)
{
    if(!ok)
        return;
    emit sigRangesReloaded();
}

void AdjustmentModuleActivator::openRMConnection()
{
    m_rmInterface = std::make_shared<Zera::Server::cRMInterface>();
    m_rmClient = Zera::Proxy::cProxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_rmInterface->setClientSmart(m_rmClient);
}

void AdjustmentModuleActivator::openPcbConnection()
{
    m_activationData->m_pcbInterface = std::make_shared<Zera::Server::cPCBInterface>();
    m_activationData->m_pcbClient = Zera::Proxy::cProxy::getInstance()->getConnectionSmart(getConfData()->m_PCBSocket.m_sIP, getConfData()->m_PCBSocket.m_nPort);
    m_activationData->m_pcbInterface->setClientSmart(m_activationData->m_pcbClient);
}

cAdjustmentModuleConfigData *AdjustmentModuleActivator::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_configuration.get())->getConfigurationData();
}

}
