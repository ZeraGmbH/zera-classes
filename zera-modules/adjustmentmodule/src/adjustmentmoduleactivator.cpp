#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"

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
    if(!openRMConnection())
        return;
    if(!sendRmIdent())
        return;
    if(!readResourceTypes())
        return;
    if(!readChannels())
        return;
    for(m_currentChannel = 0; m_currentChannel<getConfData()->m_nAdjustmentChannelCount; m_currentChannel++) {
        if(!readPortNo(m_currentChannel)) // port
            return;
        if(!openPcbConnection(m_currentChannel))
            return;
        if(!readChannelAlias(m_currentChannel))
            return;
        if(!regNotifier(m_currentChannel))
            return;
        if(!readRangeList(m_currentChannel))
            return;
    }
    m_bActive = true;
    emit sigActivationReady();
}

void AdjustmentModuleActivator::deactivate()
{
    for(int pcbInterfaceNo = 0; pcbInterfaceNo<m_activationData.m_pcbInterfaceList.count(); ++pcbInterfaceNo) {
        if(!unregNotifier(pcbInterfaceNo))
            return;
    }
    m_bActive = false;
    emit sigDeactivationReady();
}

void AdjustmentModuleActivator::setupServerResponseHandlers()
{
    setUpRmIdentHandler();
    setUpResourceTypeHandler();
    setUpReadChannelsHandler();
    setUpReadPortHandler();
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

bool AdjustmentModuleActivator::openRMConnection()
{
    m_rmClient = m_proxy->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    SignalWaiter waiter(m_rmClient.get(), &Zera::Proxy::cProxyClient::connected, 25000);
    m_rmInterface.setClientSmart(m_rmClient);
    m_proxy->startConnectionSmart(m_rmClient);
    bool ok = waiter.wait();
    if(waiter.wait())
        connect(&m_rmInterface, &Zera::Server::cRMInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
    return ok;
}

bool AdjustmentModuleActivator::sendRmIdent()
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Adjustment"))] = sendrmident;
    return waiter.wait();
}

void AdjustmentModuleActivator::setUpRmIdentHandler()
{
    m_cmdFinishCallbacks[sendrmident] = [&](quint8 reply, QVariant) {
        if (reply == ack)
            emit activationContinue();
        else
            notifyActivationError(tr(rmidentErrMSG));
    };
}

bool AdjustmentModuleActivator::readResourceTypes()
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
    return waiter.wait();
}

void AdjustmentModuleActivator::setUpResourceTypeHandler()
{
    m_cmdFinishCallbacks[readresourcetypes] = [&](quint8 reply, QVariant answer) {
        if ((reply == ack) && (answer.toString().contains("SENSE")))
            emit activationContinue();
        else
            notifyActivationError(tr(resourcetypeErrMsg));
    };
}

bool AdjustmentModuleActivator::readChannels()
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
    return waiter.wait();
}

void AdjustmentModuleActivator::setUpReadChannelsHandler()
{
    m_cmdFinishCallbacks[readresource] = [&](quint8 reply, QVariant answer) {
        bool allPresent = false;
        if (reply == ack) {
            QString s = answer.toString();
            allPresent = true;
            int n = getConfData()->m_nAdjustmentChannelCount;
            for (int i = 0; i < n; i++)
                allPresent = allPresent && s.contains(getConfData()->m_AdjChannelList.at(i));
        }
        if (allPresent)
            emit activationContinue();
        else
            notifyActivationError(tr(resourceErrMsg));
    };
}

bool AdjustmentModuleActivator::readPortNo(int channelNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", getConfData()->m_AdjChannelList.at(channelNo))] = readresourceinfo;
    return waiter.wait();
}

void AdjustmentModuleActivator::setUpReadPortHandler()
{
    m_cmdFinishCallbacks[readresourceinfo] = [&](quint8 reply, QVariant answer) {
        QStringList sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4)) {
            bool ok;
            int port = sl.at(3).toInt(&ok);
            if (ok) {
                m_activationData.m_chnPortHash[getConfData()->m_AdjChannelList.at(m_currentChannel)] = port;
                emit activationContinue();
            }
            else
                notifyActivationError(tr(resourceInfoErrMsg));
        }
        else
            notifyActivationError(tr(resourceInfoErrMsg));
    };
}

bool AdjustmentModuleActivator::openPcbConnection(int channelNo)
{
    QString sChannel = getConfData()->m_AdjChannelList.at(channelNo); // current channel m0/m1/..
    cAdjustChannelInfo* adjustChannelInfo = new cAdjustChannelInfo();
    m_activationData.m_adjustChannelInfoHash[sChannel] = adjustChannelInfo;
    int port = m_activationData.m_chnPortHash[getConfData()->m_AdjChannelList.at(channelNo)];
    if (m_activationData.m_portChannelHash.contains(port)) {
        // the channels share the same interface
        adjustChannelInfo->m_pPCBInterface = m_activationData.m_adjustChannelInfoHash[m_activationData.m_portChannelHash[port] ]->m_pPCBInterface;
        return true;
    }
    else {
        m_activationData.m_portChannelHash[port] = sChannel;
        Zera::Proxy::cProxyClient* pcbclient = m_proxy->getConnection(getConfData()->m_PCBSocket.m_sIP, port);
        m_activationData.m_pcbClientList.append(pcbclient);

        SignalWaiter waiterConnect(pcbclient, &Zera::Proxy::cProxyClient::connected, 25000);
        Zera::Server::cPCBInterface *pcbInterface = new Zera::Server::cPCBInterface();
        m_activationData.m_pcbInterfaceList.append(pcbInterface);
        pcbInterface->setClient(pcbclient);
        connect(pcbInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
        adjustChannelInfo->m_pPCBInterface = pcbInterface;
        m_proxy->startConnection((pcbclient));
        return waiterConnect.wait();
    }
}

bool AdjustmentModuleActivator::readChannelAlias(int channelNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    QString name = getConfData()->m_AdjChannelList.at(channelNo);
    m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[name]->m_pPCBInterface->getAlias(name)] = readchnalias;
    return waiter.wait();
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

bool AdjustmentModuleActivator::regNotifier(int channelNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    QString channel = getConfData()->m_AdjChannelList.at(channelNo); // current channel m0/m1/..
    Zera::Server::cPCBInterface* pcbInterface = m_activationData.m_adjustChannelInfoHash[channel]->m_pPCBInterface;
    m_MsgNrCmdList[pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(channel),"1")] = registerNotifier;
    return waiter.wait();
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

bool AdjustmentModuleActivator::readRangeList(int channelNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    QString name = getConfData()->m_AdjChannelList.at(channelNo);
    m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[name]->m_pPCBInterface->getRangeList(name)] = readrangelist;
    return waiter.wait();
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

bool AdjustmentModuleActivator::unregNotifier(int pcbInterfaceNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::deactivationContinue,
                        this, &AdjustmentModuleActivator::deactivationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_activationData.m_pcbInterfaceList[pcbInterfaceNo]->unregisterNotifiers() ] = unregisterNotifiers ;
    return waiter.wait();
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
