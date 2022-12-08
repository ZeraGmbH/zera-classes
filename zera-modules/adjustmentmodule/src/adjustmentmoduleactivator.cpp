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

AdjustmentModuleActivator::AdjustmentModuleActivator(cAdjustmentModule *module,
                                                     std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                                                     AdjustmentModuleActivateDataPtr activationData) :
    m_activationData(activationData),
    m_module(module),
    m_configuration(pConfiguration)
{
}

TaskParallelPtr AdjustmentModuleActivator::getChannelsReadTasks()
{
    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        TaskSequencePtr perChannelTasks = TaskSequence::create();
        perChannelTasks->appendTask(TaskRmReadChannelAlias::create(m_activationData, channelName,
                                                                   TRANSACTION_TIMEOUT, [&]{ emit errMsg(readaliasErrMsg); }));
        perChannelTasks->appendTask(TaskChannelRegisterNotifier::create(m_activationData, channelName,
                                                                        TRANSACTION_TIMEOUT, [&]{ emit errMsg(registerpcbnotifierErrMsg); }));
        perChannelTasks->appendTask(TaskChannelReadRanges::create(m_activationData, channelName,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(readrangelistErrMsg); }));
        parallelTasks->addTask(std::move(perChannelTasks));
    }
    return parallelTasks;
}

void AdjustmentModuleActivator::activate()
{
    if(!checkExternalVeinComponents()) // this should not be here
        return;
    openRMConnection();

    m_activationTasks.appendTask(TaskServerConnectionStart::create(m_rmClient, CONNECTION_TIMEOUT));
    m_activationTasks.appendTask(TaskRmSendIdent::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.appendTask(TaskRmCheckResourceType::create(m_rmInterface, TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.appendTask(TaskRmCheckChannelsAvail::create(m_rmInterface, getConfData()->m_AdjChannelList,
                                                                  TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceErrMsg); }));

    TaskParallelPtr parallelTasks = TaskParallel::create();
    for(const auto &channelName : qAsConst(getConfData()->m_AdjChannelList)) {
        parallelTasks->addTask(TaskReadChannelIpPort::create(m_rmInterface, channelName, m_activationData->m_chnPortHash,
                                                             TRANSACTION_TIMEOUT, [&]{ emit errMsg(resourceInfoErrMsg); }));
    }
    m_activationTasks.appendTask(std::move(parallelTasks));
    m_activationTasks.appendTask(TaskChannelPcbConnectionsStart::create(m_activationData,
                                                                        getConfData()->m_AdjChannelList,
                                                                        getConfData()->m_PCBSocket.m_sIP,
                                                                        CONNECTION_TIMEOUT));
    m_activationTasks.appendTask(getChannelsReadTasks());

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
        parallelTasks->addTask(TaskChannelUnregisterNotifier::create(m_activationData, channelName,
                                                                     TRANSACTION_TIMEOUT, [&]{ emit errMsg(unregisterpcbnotifierErrMsg); }));
    }
    m_deactivationTasks.appendTask(std::move(parallelTasks));
    connect(&m_deactivationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::deactivateContinue);
    m_deactivationTasks.start();
}

void AdjustmentModuleActivator::deactivateContinue(bool ok)
{
    if(!ok)
        return;
    emit sigDeactivationReady();
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
    m_rmClient = Zera::Proxy::cProxy::getInstance()->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    m_rmInterface->setClientSmart(m_rmClient);
}

cAdjustmentModuleConfigData *AdjustmentModuleActivator::getConfData()
{
    return qobject_cast<cAdjustmentModuleConfiguration*>(m_configuration.get())->getConfigurationData();
}

}
