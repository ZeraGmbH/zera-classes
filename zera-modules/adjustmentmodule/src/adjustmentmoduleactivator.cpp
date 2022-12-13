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
#include "taskimmediatelambda.h"
#include "errormessages.h"

namespace ADJUSTMENTMODULE
{

AdjustmentModuleActivator::AdjustmentModuleActivator(QStringList configuredChannels,
                                                     AdjustmentModuleCommonPtr activationData) :
    m_configuredChannels(configuredChannels),
    m_commonObjects(activationData)
{
    connect(&m_activationTasks, &TaskContainer::sigFinish, this, &AdjustmentModuleActivator::onActivateContinue);
    connect(&m_deactivationTasks, &TaskContainer::sigFinish, this, &AdjustmentModuleActivator::onDeactivateContinue);
    connect(&m_reloadRangesTasks, &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onReloadRanges);
}

void AdjustmentModuleActivator::activate()
{
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        m_commonObjects->m_adjustChannelInfoHash[channelName] = std::make_unique<cAdjustChannelInfo>();
    }
    addStaticActivationTasks();
    addDynChannelActivationTasks();
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
    m_deactivationTasks.addSub(getDeactivationTasks());
    m_deactivationTasks.start();
}

void AdjustmentModuleActivator::onDeactivateContinue(bool ok)
{
    if(!ok)
        return;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::reloadRanges()
{
    m_reloadRangesTasks.addSub(getChannelsReadTasks());
    m_reloadRangesTasks.start();
}

void AdjustmentModuleActivator::onReloadRanges(bool ok)
{
    if(!ok)
        return;
    emit sigRangesReloaded();
}

void ADJUSTMENTMODULE::AdjustmentModuleActivator::addStaticActivationTasks()
{
    m_activationTasks.addSub(TaskServerConnectionStart::create(m_commonObjects->m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.addSub(TaskRmSendIdent::create(m_commonObjects->m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.addSub(TaskRmCheckResourceType::create(m_commonObjects->m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.addSub(TaskRmCheckChannelsAvail::create(m_commonObjects->m_rmInterface, m_configuredChannels,
                                                              TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));
    m_activationTasks.addSub(TaskServerConnectionStart::create(m_commonObjects->m_pcbClient, CONNECTION_TIMEOUT));
}

void AdjustmentModuleActivator::addDynChannelActivationTasks()
{
    m_activationTasks.addSub(TaskImmediateLambda::create([&](){
        m_activationTasks.addSub(getChannelsReadTasks());
        m_activationTasks.addSub(getChannelsRegisterNotifyTasks());
        return true;
    }));
}

TaskCompositePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskContainerPtr channelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        TaskContainerPtr perChannelTasks = TaskSequence::create();
        perChannelTasks->addSub(TaskRmReadChannelAlias::create(m_commonObjects, channelName,
                                                               TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->addSub(TaskChannelReadRanges::create(m_commonObjects, channelName,
                                                              TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        channelTasks->addSub(std::move(perChannelTasks));
    }
    return channelTasks;
}

TaskCompositePtr ADJUSTMENTMODULE::AdjustmentModuleActivator::getChannelsRegisterNotifyTasks()
{
    TaskContainerPtr tasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        tasks->addSub(TaskChannelRegisterNotifier::create(m_commonObjects->m_pcbInterface, channelName,
                                                          TRANSACTION_TIMEOUT, [&]{ emit errMsg(registerpcbnotifierErrMsg); }));
    }
    return tasks;
}

TaskCompositePtr ADJUSTMENTMODULE::AdjustmentModuleActivator::getDeactivationTasks()
{
    return TaskChannelUnregisterNotifier::create(m_commonObjects->m_pcbInterface,
                                                 TRANSACTION_TIMEOUT, [&]{ emit errMsg(unregisterpcbnotifierErrMsg); });
}

}
