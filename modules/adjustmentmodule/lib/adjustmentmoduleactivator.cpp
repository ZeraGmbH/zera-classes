#include "adjustmentmoduleactivator.h"
#include "taskserverconnectionstart.h"
#include "taskchannelscheckavail.h"
#include "taskchannelgetalias.h"
#include "taskchannelregisternotifier.h"
#include "taskchannelgetrangelist.h"
#include "taskunregisternotifier.h"
#include "tasklambdarunner.h"
#include "errormessages.h"

AdjustmentModuleActivator::AdjustmentModuleActivator(QStringList configuredChannels,
                                                     AdjustmentModuleCommonPtr activationData) :
    m_configuredChannels(configuredChannels),
    m_commonObjects(activationData)
{
    connect(&m_activationTasks,   &TaskTemplate::sigFinish, this, &AdjustmentModuleActivator::onActivateContinue);
    connect(&m_deactivationTasks, &TaskTemplate::sigFinish, this, &AdjustmentModuleActivator::onDeactivateContinue);
    connect(&m_reloadRangesTasks, &TaskTemplate::sigFinish, this, &AdjustmentModuleActivator::onReloadRanges);
}

void AdjustmentModuleActivator::activate()
{
    initChannelInfoHash();
    addStaticActivationTasks();
    addDynChannelActivationTasks();
    m_activationTasks.start();
}

void AdjustmentModuleActivator::onActivateContinue(bool ok)
{
    if(ok) {
        fillChannelAliasHash();
        emit sigActivationReady();
    }
}

void AdjustmentModuleActivator::deactivate()
{
    m_deactivationTasks.addSub(getDeactivationTasks());
    m_deactivationTasks.start();
}

void AdjustmentModuleActivator::onDeactivateContinue(bool ok)
{
    if(ok)
        emit sigDeactivationReady();
}

void AdjustmentModuleActivator::reloadRanges()
{
    m_reloadRangesTasks.addSub(getChannelsReadTasks());
    m_reloadRangesTasks.start();
}

void AdjustmentModuleActivator::onReloadRanges(bool ok)
{
    if(ok)
        emit sigRangesReloaded();
}

void AdjustmentModuleActivator::addStaticActivationTasks()
{
    m_activationTasks.addSub(TaskServerConnectionStart::create(
                                 m_commonObjects->m_pcbClient,
                                 CONNECTION_TIMEOUT));
    m_activationTasks.addSub(TaskChannelsCheckAvail::create(
                                 m_commonObjects->m_pcbInterface,
                                 m_configuredChannels,
                                 TRANSACTION_TIMEOUT,[&]{ notifyError(resourceErrMsg); }));
}

void AdjustmentModuleActivator::addDynChannelActivationTasks()
{
    m_activationTasks.addSub(TaskLambdaRunner::create([&](){
        m_activationTasks.addSub(getChannelsReadTasks());
        m_activationTasks.addSub(getChannelsRegisterNotifyTasks());
        return true;
    }));
}

void AdjustmentModuleActivator::notifyError(QVariant value)
{
    emit errMsg(value);
}

TaskTemplatePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskContainerInterfacePtr channelTasks = TaskContainerParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        TaskContainerInterfacePtr perChannelTasks = TaskContainerSequence::create();
        perChannelTasks->addSub(TaskChannelGetAlias::create(
                                    m_commonObjects->m_pcbInterface,
                                    channelName,
                                    m_commonObjects->m_adjustChannelInfoHash[channelName]->m_sAlias,
                                    TRANSACTION_TIMEOUT, [&]{ notifyError(readaliasErrMsg); }));
        perChannelTasks->addSub(TaskChannelGetRangeList::create(
                                    m_commonObjects->m_pcbInterface,
                                    channelName,
                                    m_commonObjects->m_adjustChannelInfoHash[channelName]->m_sRangelist,
                                    TRANSACTION_TIMEOUT,[&]{ notifyError(readrangelistErrMsg); }));
        channelTasks->addSub(std::move(perChannelTasks));
    }
    return channelTasks;
}

TaskTemplatePtr AdjustmentModuleActivator::getChannelsRegisterNotifyTasks()
{
    TaskContainerInterfacePtr tasks = TaskContainerParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels))
        tasks->addSub(TaskChannelRegisterNotifier::create(
                          m_commonObjects->m_pcbInterface,
                          channelName,
                          TRANSACTION_TIMEOUT, [&]{ notifyError(registerpcbnotifierErrMsg); }));
    return tasks;
}

TaskTemplatePtr AdjustmentModuleActivator::getDeactivationTasks()
{
    return TaskUnregisterNotifier::create(
                m_commonObjects->m_pcbInterface,
                TRANSACTION_TIMEOUT, [&]{ notifyError(unregisterpcbnotifierErrMsg); });
}

void AdjustmentModuleActivator::initChannelInfoHash()
{
    for(const auto &channelName : qAsConst(m_configuredChannels))
        m_commonObjects->m_adjustChannelInfoHash[channelName] = std::make_unique<AdjustChannelInfo>();
}

void AdjustmentModuleActivator::fillChannelAliasHash()
{
    for(const auto& entry : m_commonObjects->m_adjustChannelInfoHash)
       m_commonObjects->m_channelAliasHash[entry.second->m_sAlias] = entry.first;
}

