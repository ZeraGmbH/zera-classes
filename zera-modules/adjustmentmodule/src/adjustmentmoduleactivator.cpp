#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"
#include "taskserverconnectionstart.h"
#include "taskrmsendident.h"
#include "taskrmcheckresourcetype.h"
#include "taskrmcheckchannelsavail.h"
#include "taskchannelreadalias.h"
#include "taskchannelregisternotifier.h"
#include "taskchannelreadranges.h"
#include "taskunregisternotifier.h"
#include "taskimmediatelambda.h"
#include "errormessages.h"

namespace ADJUSTMENTMODULE
{

AdjustmentModuleActivator::AdjustmentModuleActivator(QStringList configuredChannels,
                                                     AdjustmentModuleCommonPtr activationData) :
    m_configuredChannels(configuredChannels),
    m_commonObjects(activationData)
{
    connect(&m_activationTasks,   &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onActivateContinue);
    connect(&m_deactivationTasks, &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onDeactivateContinue);
    connect(&m_reloadRangesTasks, &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onReloadRanges);
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
        perChannelTasks->addSub(TaskChannelReadAlias::create(m_commonObjects->m_pcbInterface, channelName,
                                                             m_commonObjects->m_adjustChannelInfoHash[channelName]->m_sAlias,
                                                             TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->addSub(TaskChannelReadRanges::create(m_commonObjects->m_pcbInterface, channelName,
                                                              m_commonObjects->m_adjustChannelInfoHash[channelName]->m_sRangelist,
                                                              TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        channelTasks->addSub(std::move(perChannelTasks));
    }
    return channelTasks;
}

TaskCompositePtr AdjustmentModuleActivator::getChannelsRegisterNotifyTasks()
{
    TaskContainerPtr tasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels))
        tasks->addSub(TaskChannelRegisterNotifier::create(m_commonObjects->m_pcbInterface, channelName,
                                                          TRANSACTION_TIMEOUT, [&]{ emit errMsg(registerpcbnotifierErrMsg); }));
    return tasks;
}

TaskCompositePtr AdjustmentModuleActivator::getDeactivationTasks()
{
    return TaskUnregisterNotifier::create(m_commonObjects->m_pcbInterface,
                                          TRANSACTION_TIMEOUT, [&]{ emit errMsg(unregisterpcbnotifierErrMsg); });
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

}
