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

AdjustmentModuleActivator::AdjustmentModuleActivator(QStringList configuredChannels,
                                                     AdjustmentModuleCommonPtr activationData) :
    m_configuredChannels(configuredChannels),
    m_commonObjects(activationData)
{
    connect(&m_activationTasks, &TaskContainer::sigFinish, this, &AdjustmentModuleActivator::onActivateContinue);
    connect(&m_deactivationTasks, &TaskContainer::sigFinish, this, &AdjustmentModuleActivator::onDeactivateContinue);
    connect(&m_reloadRangesTasks, &TaskComposite::sigFinish, this, &AdjustmentModuleActivator::onReloadRanges);
}

TaskCompositePtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskContainerPtr channelReadTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        TaskContainerPtr perChannelTasks = TaskSequence::create();
        perChannelTasks->addSubTask(TaskRmReadChannelAlias::create(m_commonObjects, channelName,
                                                                   TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->addSubTask(TaskChannelReadRanges::create(m_commonObjects, channelName,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        channelReadTasks->addSubTask(std::move(perChannelTasks));
    }
    return channelReadTasks;
}

void AdjustmentModuleActivator::activate()
{
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        m_commonObjects->m_adjustChannelInfoHash[channelName] = std::make_unique<cAdjustChannelInfo>();
    }
    m_activationTasks.addSubTask(TaskServerConnectionStart::create(m_commonObjects->m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.addSubTask(TaskRmSendIdent::create(m_commonObjects->m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.addSubTask(TaskRmCheckResourceType::create(m_commonObjects->m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.addSubTask(TaskRmCheckChannelsAvail::create(m_commonObjects->m_rmInterface, m_configuredChannels,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));
    m_activationTasks.addSubTask(TaskServerConnectionStart::create(m_commonObjects->m_pcbClient, CONNECTION_TIMEOUT));

    m_activationTasks.addSubTask(getChannelsReadTasks());
    TaskContainerPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        parallelTasks->addSubTask(TaskChannelRegisterNotifier::create(m_commonObjects, channelName,
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
    TaskContainerPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(m_configuredChannels)) {
        parallelTasks->addSubTask(TaskChannelUnregisterNotifier::create(m_commonObjects, channelName,
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

}
