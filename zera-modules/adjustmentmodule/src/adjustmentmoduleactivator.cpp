#include "adjustmentmoduleactivator.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjustmentmoduleconfiguration.h"

#include "errormessages.h"
#include "reply.h"

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

void AdjustmentModuleActivator::setUpActivationStateMachine()
{
    // m_rmConnectState.addTransition is done in rmConnectState entered handler
    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.setInitialState(&m_rmConnectState);
    connect(&m_rmConnectState, &QState::entered, this, [&]() {
        // we instantiate a working resource manager interface first
        // so first we try to get a connection to resource manager over proxy
        m_pRMClient = m_proxy->getConnection(getConfData()->m_RMSocket.m_sIP, getConfData()->m_RMSocket.m_nPort);
        m_rmConnectState.addTransition(m_pRMClient, &Zera::Proxy::cProxyClient::connected, &m_IdentifyState);
        // and then we set connection resource manager interface's connection
        m_rmInterface.setClient(m_pRMClient);
        connect(&m_rmInterface, &Zera::Server::cRMInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
        m_proxy->startConnection(m_pRMClient);
    });

    m_activationMachine.addState(&m_IdentifyState);
    m_IdentifyState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceTypesState);
    connect(&m_IdentifyState, &QState::entered, this, [&]() {
        m_MsgNrCmdList[m_rmInterface.rmIdent(QString("Adjustment"))] = sendrmident;
    });
    m_cmdFinishCallbacks[sendrmident] = [&](quint8 reply, QVariant) {
        if (reply == ack) // we only continue if resource manager acknowledges
            emit activationContinue();
        else
            notifyActivationError(tr(rmidentErrMSG));
    };

    m_activationMachine.addState(&m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceState);
    connect(&m_readResourceTypesState, &QState::entered, this, [&]() {
        m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
    });
    m_cmdFinishCallbacks[readresourcetypes] = [&](quint8 reply, QVariant answer) {
        if ((reply == ack) && (answer.toString().contains("SENSE")))
            emit activationContinue();
        else
            notifyActivationError(tr(resourcetypeErrMsg));
    };

    m_activationMachine.addState(&m_readResourceState);
    m_readResourceState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceInfoState);
    connect(&m_readResourceState, &QState::entered, this, [&]() {
        m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
        activationIt = 0; // we prepare querying loop of channels
    });
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

    m_activationMachine.addState(&m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readResourceInfoLoopState);
    connect(&m_readResourceInfoState, &QState::entered, this, [&]() {
        m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", getConfData()->m_AdjChannelList.at(activationIt))] = readresourceinfo;
    });
    m_cmdFinishCallbacks[readresourceinfo] = [&](quint8 reply, QVariant answer) {
        QStringList sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4)) {
            bool ok;
            int port = sl.at(3).toInt(&ok);
            if (ok) {
                m_activationData.m_chnPortHash[getConfData()->m_AdjChannelList.at(activationIt)] = port;
                emit activationContinue();
            }
            else
                notifyActivationError(tr(resourceInfoErrMsg));
        }
        else
            notifyActivationError(tr(resourceInfoErrMsg));
    };

    m_activationMachine.addState(&m_readResourceInfoLoopState);
    m_readResourceInfoLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_pcbConnectionState);
    m_readResourceInfoLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readResourceInfoState);
    connect(&m_readResourceInfoLoopState, &QState::entered, this, [&]() {
        activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
        if (activationIt == 0)
            emit activationContinue();
        else
            emit activationLoop();
    });

    m_activationMachine.addState(&m_pcbConnectionState);
    m_pcbConnectionState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_pcbConnectionLoopState);
    connect(&m_pcbConnectionState, &QState::entered, this, [&] {
        QString sChannel = getConfData()->m_AdjChannelList.at(activationIt); // current channel m0/m1/..
        cAdjustChannelInfo* adjustChannelInfo = new cAdjustChannelInfo();
        m_activationData.m_adjustChannelInfoHash[sChannel] = adjustChannelInfo;
        int port = m_activationData.m_chnPortHash[getConfData()->m_AdjChannelList.at(activationIt)];
        if (m_activationData.m_portChannelHash.contains(port)) {
            // the channels share the same interface
            adjustChannelInfo->m_pPCBInterface = m_activationData.m_adjustChannelInfoHash[m_activationData.m_portChannelHash[port] ]->m_pPCBInterface;
            emit activationContinue();
        }
        else {
            m_activationData.m_portChannelHash[port] = sChannel;
            Zera::Proxy::cProxyClient* pcbclient = m_proxy->getConnection(getConfData()->m_PCBSocket.m_sIP, port);
            m_activationData.m_pcbClientList.append(pcbclient);
            m_pcbConnectionState.addTransition(pcbclient, &Zera::Proxy::cProxyClient::connected, &m_pcbConnectionLoopState);
            Zera::Server::cPCBInterface *pcbInterface = new Zera::Server::cPCBInterface();
            m_activationData.m_pcbInterfaceList.append(pcbInterface);
            pcbInterface->setClient(pcbclient);
            connect(pcbInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &AdjustmentModuleActivator::catchInterfaceAnswer);
            adjustChannelInfo->m_pPCBInterface = pcbInterface;
            m_proxy->startConnection((pcbclient));
        }
    });

    m_activationMachine.addState(&m_pcbConnectionLoopState);
    m_pcbConnectionLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readChnAliasState);
    m_pcbConnectionLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_pcbConnectionState);
    connect(&m_pcbConnectionLoopState, &QState::entered, this, [&]() {
        activationIt = (activationIt + 1) % getConfData()->m_nAdjustmentChannelCount;
        if (activationIt == 0)
            emit activationContinue();
        else
            emit activationLoop();

    });

    m_activationMachine.addState(&m_readChnAliasState);
    m_readChnAliasState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readChnAliasLoopState);
    connect(&m_readChnAliasState, &QState::entered, this, [&]() {
        QString name = getConfData()->m_AdjChannelList.at(activationIt);
        m_MsgNrCmdList[m_activationData.m_adjustChannelInfoHash[name]->m_pPCBInterface->getAlias(name)] = readchnalias;
    });
    m_cmdFinishCallbacks[readchnalias] = [&](quint8 reply, QVariant answer) {
        if (reply == ack) {
            QString alias = answer.toString();
            QString sysName = getConfData()->m_AdjChannelList.at(activationIt);
            m_activationData.m_AliasChannelHash[alias] = sysName;
            m_activationData.m_adjustChannelInfoHash[sysName]->m_sAlias = alias;
            emit activationContinue();
        }
        else
            notifyActivationError(tr(readaliasErrMsg));
    };

    m_activationMachine.addState(&m_readChnAliasLoopState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationContinue, &m_readRangelistState);
    m_readChnAliasLoopState.addTransition(this, &cAdjustmentModuleMeasProgram::activationLoop, &m_readChnAliasState);
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
    m_deactivateState.addTransition(this, &cAdjustmentModuleMeasProgram::deactivationContinue, &m_deactivateDoneState);
    connect(&m_deactivateState, &QState::entered, this, [&]() {
        m_bActive = false;
        emit deactivationContinue();
    });

    m_deactivationMachine.addState(&m_deactivateDoneState);
    connect(&m_deactivateDoneState, &QState::entered, this, [&]() {
        m_proxy->releaseConnection(m_pRMClient);
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
