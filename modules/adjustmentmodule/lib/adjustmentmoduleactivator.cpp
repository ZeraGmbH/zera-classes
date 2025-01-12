#include "adjustmentmoduleactivator.h"
#include "taskchannelgetalias.h"
#include "taskchannelgetrangelist.h"
#include "taskregisternotifier.h"
#include "taskunregisternotifier.h"
#include "tasklambdarunner.h"
#include "errormessages.h"
#include <taskcontainerparallel.h>

AdjustmentModuleActivator::AdjustmentModuleActivator(QStringList configuredChannels,
                                                     AdjustmentModuleCommonPtr activationData,
                                                     QString moduleName) :
    m_moduleName(moduleName),
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
    m_activationTasks.addSub(m_commonObjects->m_pcbConnection.createConnectionTask());
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
    qWarning("Module %s reported error: %s", qPrintable(m_moduleName), qPrintable(value.toString()));
}

TaskTemplatePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskContainerInterfacePtr channelTasks = TaskContainerParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        TaskContainerInterfacePtr perChannelTasks = TaskContainerSequence::create();
        perChannelTasks->addSub(TaskChannelGetAlias::create(
                                    m_commonObjects->m_pcbConnection.getInterface(),
                                    channelName,
                                    m_commonObjects->m_adjustChannelInfoHash[channelName]->m_sAlias,
                                    TRANSACTION_TIMEOUT, [&]{ notifyError(readaliasErrMsg); }));
        perChannelTasks->addSub(TaskChannelGetRangeList::create(
                                    m_commonObjects->m_pcbConnection.getInterface(),
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
    // Hmm we ignore id and relaoad all - see cAdjustmentModuleMeasProgram::catchInterfaceAnswer
    constexpr int notifierId = 1;
    for(const auto &channelName : qAsConst(m_configuredChannels))
        tasks->addSub(TaskRegisterNotifier::create(
                        m_commonObjects->m_pcbConnection.getInterface(),
                        QString("SENS:%1:RANG:CAT?").arg(channelName), notifierId,
                        TRANSACTION_TIMEOUT, [&]{ notifyError(registerpcbnotifierErrMsg); }));
    return tasks;
}

TaskTemplatePtr AdjustmentModuleActivator::getDeactivationTasks()
{
    return TaskUnregisterNotifier::create(
                m_commonObjects->m_pcbConnection.getInterface(),
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

