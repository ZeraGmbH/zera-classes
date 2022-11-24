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
    if(!openRMConnection())
        return;
    if(!sendRmIdent())
        return;
    if(!readResourceTypes())
        return;
    if(!readChannels())
        return;
    for(m_loopCountForHandler = 0; m_loopCountForHandler<getConfData()->m_nAdjustmentChannelCount; m_loopCountForHandler++) {
        if(!readPortNo(m_loopCountForHandler)) // port
            return;
        if(!openPcbConnection(m_loopCountForHandler))
            return;
        if(!readChannelAlias(m_loopCountForHandler))
            return;

    }
    m_activationMachine.start();
}

void AdjustmentModuleActivator::setupServerResponseHandlers()
{
    setUpRmIdentHandler();
    setUpResourceTypeHandler();
    setUpReadChannelsHandler();
    setUpReadPortHandler();
    setUpReadChannelAliasHandler();
}

bool AdjustmentModuleActivator::openRMConnection()
{
    m_rmClient = m_proxy->getConnectionSmart(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
    SignalWaiter waiter(m_rmClient.get(), &Zera::Proxy::cProxyClient::connected, 25000);
    m_rmInterface.setClientSmart(m_rmClient);
    m_proxy->startConnectionSmart(m_rmClient);
    SignalWaiter::WaitResult res = waiter.wait();
    bool ok = (res == SignalWaiter::WAIT_OK_SIG);
    if(ok)
        connect(&m_rmInterface, &Zera::Server::cRMInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
    return ok;
}

bool AdjustmentModuleActivator::sendRmIdent()
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Adjustment"))] = sendrmident;
    return waiter.wait() == SignalWaiter::WAIT_OK_SIG;
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
    return waiter.wait() == SignalWaiter::WAIT_OK_SIG;
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
    return waiter.wait() == SignalWaiter::WAIT_OK_SIG;
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
    return waiter.wait() == SignalWaiter::WAIT_OK_SIG;
}

void AdjustmentModuleActivator::setUpReadPortHandler()
{
    m_cmdFinishCallbacks[readresourceinfo] = [&](quint8 reply, QVariant answer) {
        QStringList sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4)) {
            bool ok;
            int port = sl.at(3).toInt(&ok);
            if (ok) {
                m_activationData.m_chnPortHash[getConfData()->m_AdjChannelList.at(m_loopCountForHandler)] = port;
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
        return waiterConnect.wait() == SignalWaiter::WAIT_OK_SIG;
    }
}

bool AdjustmentModuleActivator::readChannelAlias(int channelNo)
{
    SignalWaiter waiter(this, &AdjustmentModuleActivator::activationContinue,
                        this, &AdjustmentModuleActivator::activationError,
                        TRANSACTION_TIMEOUT);
    QString name = getConfData()->m_AdjChannelList.at(channelNo);
    m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[name]->m_pPCBInterface->getAlias(name)] = readchnalias;
    return waiter.wait() == SignalWaiter::WAIT_OK_SIG;
}

void AdjustmentModuleActivator::setUpReadChannelAliasHandler()
{
    m_cmdFinishCallbacks[readchnalias] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            QString alias = answer.toString();
            QString sysName = getConfData()->m_AdjChannelList.at(m_loopCountForHandler);
            m_activationData.m_AliasChannelHash[alias] = sysName;
            m_activationData.m_adjustChannelInfoHash[sysName]->m_sAlias = alias;
            emit activationContinue();
        }
        else
            notifyActivationError(tr(readaliasErrMsg));
    };
}




void AdjustmentModuleActivator::deactivate()
{
    m_deactivationMachine.start();
}

void AdjustmentModuleActivator::setUpActivationStateMachine()
{
    m_activationMachine.addState(&m_registerNotifier);
    m_activationMachine.setInitialState(&m_registerNotifier);
    m_registerNotifier.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readChnAliasLoopState);
    connect(&m_registerNotifier, &QState::entered, this, [&]() {
        QString channel = getConfData()->m_AdjChannelList.at(activationIt); // current channel m0/m1/..
        Zera::Server::cPCBInterface* pcbInterface = m_activationData.m_adjustChannelInfoHash[channel]->m_pPCBInterface;
        m_MsgNrCmdList[pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(channel),"1")] = registerNotifier;
    });
    m_cmdFinishCallbacks[registerNotifier] = [&](quint8 reply, QVariant) {
        if (reply == ack)
            emit activationContinue();
        else
            notifyActivationError(tr(registerpcbnotifierErrMsg));
    };

    m_activationMachine.addState(&m_readChnAliasLoopState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readRangelistState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_registerNotifier);
    connect(&m_readChnAliasLoopState, &QState::entered, this, [&]() {
        activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
        if (activationIt == 0)
            emit activationContinue();
        else
            emit activationLoop();
    });

    m_activationMachine.addState(&m_readRangelistState);
    m_readRangelistState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readRangelistLoopState);
    connect(&m_readRangelistState, &QState::entered, this, [&]() {
        QString name = getConfData()->m_AdjChannelList.at(activationIt);
        m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[name]->m_pPCBInterface->getRangeList(name)] = readrangelist;
    });
    m_cmdFinishCallbacks[readrangelist] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            m_activationData.m_adjustChannelInfoHash[getConfData()->m_AdjChannelList.at(activationIt)]->m_sRangelist = answer.toStringList();
            emit activationContinue();
        }
        else
            notifyActivationError(tr(readrangelistErrMsg));
    };

    m_activationMachine.addState(&m_readRangelistLoopState);
    m_readRangelistLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_searchActualValuesState);
    m_readRangelistLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readRangelistState);
    connect(&m_readRangelistLoopState, &QState::entered, this, [&]() {
        activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
        if (activationIt == 0)
            emit activationContinue();
        else
            emit activationLoop();
    });

    m_activationMachine.addState(&m_searchActualValuesState);
    m_searchActualValuesState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_activationDoneState);
    connect(&m_searchActualValuesState, &QState::entered, this, [&] () {
        bool error = false;
        adjInfoType adjInfo = getConfData()->m_ReferenceAngle;
        if (!m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            error = true;
        adjInfo = getConfData()->m_ReferenceFrequency;
        if (!m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent))
            error = true;
        for (int i = 0; i < getConfData()->m_nAdjustmentChannelCount; i++) {
            // we test if all configured actual value data exist
            QString chn = getConfData()->m_AdjChannelList.at(i);

            adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->amplitudeAdjInfo;
            if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
                error = true;
                break;
            }
            adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->phaseAdjInfo;
            if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
                error = true;
                break;
            }
            adjInfo = getConfData()->m_AdjChannelInfoHash[chn]->offsetAdjInfo;
            if (adjInfo.m_bAvail && !m_module->m_pStorageSystem->hasStoredValue(adjInfo.m_nEntity, adjInfo.m_sComponent)) {
                error = true;
                break;
            }
        }
        if (error)
            notifyActivationError(tr(moduleActValueErrMsg));
        else
            emit activationContinue();
    });

    m_activationMachine.addState(&m_activationDoneState);
    connect(&m_activationDoneState, &QState::entered, this, [&]() {
        m_bActive = true;
        emit sigActivationReady();
    });
}

void AdjustmentModuleActivator::setUpDeactivationStateMachine()
{
    m_deactivationMachine.addState(&m_deactivateState);
    m_deactivationMachine.setInitialState(&m_deactivateState);
    m_deactivateState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationContinue, &m_unregisterState);
    m_deactivateState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationSkip, &m_deactivateDoneState);
    connect(&m_deactivateState, &QState::entered, this, [&]() {
        m_bActive = false;
        deactivationIt = m_activationData.m_pcbInterfaceList.constBegin();
        if (deactivationIt != m_activationData.m_pcbInterfaceList.constEnd())
            emit deactivationContinue();
        else
            emit deactivationSkip();
    });

    m_deactivationMachine.addState(&m_unregisterState);
    m_unregisterState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationContinue, &m_unregisterLoopState);
    connect(&m_unregisterState, &QState::entered, this, [&]() {
       m_MsgNrCmdList[(*deactivationIt)->unregisterNotifiers() ] = unregisterNotifiers ;
    });
    m_cmdFinishCallbacks[unregisterNotifiers] = [&](quint8 reply, QVariant answer) {
        if (reply == ack)
            emit deactivationContinue();
        else
            notifyActivationError(tr(unregisterpcbnotifierErrMsg));
    };

    m_deactivationMachine.addState(&m_unregisterLoopState);
    m_unregisterLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);
    m_unregisterLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationLoop, &m_unregisterState);
    connect(&m_unregisterLoopState, &QState::entered, this, [&]() {
        deactivationIt++;
        if (deactivationIt == m_activationData.m_pcbInterfaceList.constEnd())
            emit deactivationContinue();
        else
            emit deactivationLoop();
    });

    m_deactivationMachine.addState(&m_deactivateDoneState);
    connect(&m_deactivateDoneState, &QState::entered, this, [&]() {
        emit sigDeactivationReady();
    });
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
