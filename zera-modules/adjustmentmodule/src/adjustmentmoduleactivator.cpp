#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"
#include "tasktimeoutdecorator.h"
#include "taskrmconnectionstart.h"
#include "taskrmsendident.h"
#include "taskrmcheckresourcetype.h"
#include "taskrmreadchannels.h"

#include "errormessages.h"
#include "reply.h"
#include "signalwaiter.h"

namespace ADJUSTMENTMODULE
{

AdjustmentModuleActivator::AdjustmentModuleActivator(cAdjustmentModule *module,
                                                     Zera::Proxy::cProxy *proxy,
                                                     std::shared_ptr<cBaseModuleConfiguration> pConfiguration,
                                                     AdjustmentModuleActivateData &activationData) :
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

    m_activationTasks.appendTask(TaskTimeoutDecorator::wrapTimeout(CONNECTION_TIMEOUT,
                                                                   TaskRmConnectionStart::create(m_rmClient)));
    m_activationTasks.appendTask(TaskTimeoutDecorator::wrapTimeout(TRANSACTION_TIMEOUT,
                                                                   TaskRmSendIdent::create(m_rmInterface),
                                                                   [&]{ emit errMsg(rmidentErrMSG); }));
    m_activationTasks.appendTask(TaskTimeoutDecorator::wrapTimeout(TRANSACTION_TIMEOUT,
                                                                   TaskRmCheckResourceType::create(m_rmInterface),
                                                                   [&]{ emit errMsg(resourcetypeErrMsg); }));
    m_activationTasks.appendTask(TaskTimeoutDecorator::wrapTimeout(TRANSACTION_TIMEOUT,
                                                                   TaskRmReadChannels::create(m_rmInterface, getConfData()->m_AdjChannelList),
                                                                   [&]{ emit errMsg(resourceErrMsg); }));

    connect(&m_activationTasks, &TaskSequence::sigFinish, this, &AdjustmentModuleActivator::activateContinue);
    m_activationTasks.start();
}

void AdjustmentModuleActivator::activateContinue(bool ok)
{
    if(!ok)
        return;
    connect(m_rmInterface.get(), &Zera::Server::cRMInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
    for(m_currentChannel = 0; m_currentChannel<getConfData()->m_nAdjustmentChannelCount; m_currentChannel++) {
        // Alarm: This assumes channels configured elsewhere having same index :(
        QString channelName = getConfData()->m_AdjChannelList.at(m_currentChannel); // current channel m0/m1/
        if(!readIpPortNo(channelName)->wait())
            return;
        if(!openPcbConnection(channelName)->wait())
            return;
        if(!readChannelAlias(channelName)->wait())
            return;
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
    for(int pcbInterfaceNo = 0; pcbInterfaceNo<m_activationData.m_pcbInterfaceList.count(); ++pcbInterfaceNo) {
        if(!unregNotifier(pcbInterfaceNo)->wait())
            return;
    }
    m_bActive = false;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::setupServerResponseHandlers()
{
    setUpReadIpPortHandler();
    setUpReadChannelAliasHandler();
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

BlockedWaitInterfacePtr AdjustmentModuleActivator::readIpPortNo(QString channelName)
{
    BlockedWaitInterfacePtr waiter =
            std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::activationContinue,
                                           this, &AdjustmentModuleActivator::activationError,
                                           TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface->getResourceInfo("SENSE", channelName)] = readresourceinfo;
    return waiter;
}

void AdjustmentModuleActivator::setUpReadIpPortHandler()
{
    m_cmdFinishCallbacks[readresourceinfo] = [&](quint8 reply, QVariant answer) {
        QStringList sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4)) {
            bool ok;
            int port = sl.at(3).toInt(&ok);
            if (ok) {
                m_chnPortHash[getConfData()->m_AdjChannelList.at(m_currentChannel)] = port;
                emit activationContinue();
            }
            else
                notifyActivationError(tr(resourceInfoErrMsg));
        }
        else
            notifyActivationError(tr(resourceInfoErrMsg));
    };
}

BlockedWaitInterfacePtr AdjustmentModuleActivator::openPcbConnection(QString channelName)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>();
    cAdjustChannelInfo* adjustChannelInfo = new cAdjustChannelInfo();
    m_activationData.m_adjustChannelInfoHash[channelName] = adjustChannelInfo;
    int port = m_chnPortHash[channelName];
    if (m_activationData.m_portChannelHash.contains(port)) {
        // the channels share the same interface
        adjustChannelInfo->m_pPCBInterface = m_activationData.m_adjustChannelInfoHash[m_activationData.m_portChannelHash[port] ]->m_pPCBInterface;
    }
    else {
        m_activationData.m_portChannelHash[port] = channelName;
        Zera::Proxy::cProxyClient* pcbclient = m_proxy->getConnection(getConfData()->m_PCBSocket.m_sIP, port);
        m_activationData.m_pcbClientList.append(pcbclient);

        waiter = std::make_unique<SignalWaiter>(pcbclient, &Zera::Proxy::cProxyClient::connected, CONNECTION_TIMEOUT);
        Zera::Server::cPCBInterface *pcbInterface = new Zera::Server::cPCBInterface();
        m_activationData.m_pcbInterfaceList.append(pcbInterface);
        pcbInterface->setClient(pcbclient);
        connect(pcbInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
        adjustChannelInfo->m_pPCBInterface = pcbInterface;
        m_proxy->startConnection((pcbclient));
    }
    return waiter;
}

BlockedWaitInterfacePtr AdjustmentModuleActivator::readChannelAlias(QString channelName)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::activationContinue,
                                                                    this, &AdjustmentModuleActivator::activationError,
                                                                    TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[channelName]->m_pPCBInterface->getAlias(channelName)] = readchnalias;
    return waiter;
}

void AdjustmentModuleActivator::setUpReadChannelAliasHandler()
{
    m_cmdFinishCallbacks[readchnalias] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            QString alias = answer.toString();
            QString sysName = getConfData()->m_AdjChannelList.at(m_currentChannel);
            m_activationData.m_AliasChannelHash[alias] = sysName;
            m_activationData.m_adjustChannelInfoHash[sysName]->m_sAlias = alias;
            emit activationContinue();
        }
        else
            notifyActivationError(tr(readaliasErrMsg));
    };
}

BlockedWaitInterfacePtr AdjustmentModuleActivator::regNotifier(QString channelName)
{
    BlockedWaitInterfacePtr waiter = std::make_unique<SignalWaiter>(this, &AdjustmentModuleActivator::activationContinue,
                                                                    this, &AdjustmentModuleActivator::activationError,
                                                                    TRANSACTION_TIMEOUT);
    Zera::Server::cPCBInterface* pcbInterface = m_activationData.m_adjustChannelInfoHash[channelName]->m_pPCBInterface;
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
    m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[channelName]->m_pPCBInterface->getRangeList(channelName)] = readrangelist;
    return waiter;
}

void AdjustmentModuleActivator::setUpRangeListHandler()
{
    m_cmdFinishCallbacks[readrangelist] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            m_activationData.m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(m_currentChannel)]->m_sRangelist = answer.toStringList();
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
    m_MsgNrCmdList[m_activationData.m_pcbInterfaceList[pcbInterfaceNo]->unregisterNotifiers() ] = unregisterNotifiers ;
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
