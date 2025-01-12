#include "samplechannel.h"
#include "samplemodule.h"
#include "samplemoduleconfigdata.h"
#include <errormessages.h>
#include <scpiinfo.h>
#include <stringvalidator.h>
#include <scpiinfo.h>
#include <vfmoduleparameter.h>
#include <reply.h>
#include <proxy.h>

namespace SAMPLEMODULE
{

cSampleChannel::cSampleChannel(cSampleModule* module, cSampleModuleConfigData& configdata) :
    cModuleActivist(QString("%1/SampleChannel No 1").arg(module->getVeinModuleName())),
    m_sName(configdata.m_ObsermaticConfPar.m_sSampleSystem),
    m_pModule(module),
    m_ConfigData(configdata),
    m_pPCBInterface(std::make_shared<Zera::cPCBInterface>())
{
    // setting up statemachine for "activating" sample channel
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readChnAliasState.addTransition(this, &cSampleChannel::activationContinue, &m_readRangelistState);
    m_readRangelistState.addTransition(this, &cSampleChannel::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readChnAliasState);

    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_pcbConnectionState);

    connect(&m_pcbConnectionState, &QState::entered, this, &cSampleChannel::pcbConnection);
    connect(&m_readChnAliasState, &QState::entered, this, &cSampleChannel::readChnAlias);
    connect(&m_readRangelistState, &QState::entered, this, &cSampleChannel::readRangelist);
    connect(&m_activationDoneState, &QState::entered, this, &cSampleChannel::activationDone);

    // setting up statemachine for "deactivating" meas channel
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_deactivationDoneState);

    connect(&m_deactivationDoneState, &QState::entered, this, &cSampleChannel::deactivationDone);
}


cSampleChannel::~cSampleChannel()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient.get());
}


void cSampleChannel::generateVeinInterface()
{
    QString key;
    cSCPIInfo *scpiInfo;

    m_pChannelRange = new VfModuleParameter(m_pModule->getEntityId(), m_pModule->m_pModuleValidator,
                                               key = QString("PAR_ChannelRange"),
                                               QString("Sampling channel's range"),
                                               QVariant(m_ConfigData.m_ObsermaticConfPar.m_pllRange.m_sPar));

    m_pModule->m_veinModuleParameterMap[key] = m_pChannelRange; // for modules use

    scpiInfo = new cSCPIInfo("CONFIGURATION", "SRANGE", "10", m_pChannelRange->getName(), "0", "");
    m_pChannelRange->setSCPIInfo(scpiInfo);
    // later we still have to set the validator for this component
}



void cSampleChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    int cmd = m_MsgNrCmdList.take(msgnr);

    switch (cmd)
    {
    case readchnaliassamplechannel:
        if (reply == ack) {
            m_sAlias = answer.toString();
            emit activationContinue();
        }
        else
            notifyError(readaliasErrMsg);
        break;
    case readrangelistsamplechannel:
        if (reply == ack) {
            m_RangeNameList = answer.toStringList();
            emit activationContinue();
        }
        else
            notifyError(readrangelistErrMsg);
        break;
    case setsamplechannelrange:
        if (reply == ack)
            m_sActRange = m_sNewRange;
        else
            notifyError(setRangeErrMsg);
        break;
    }
}

QString cSampleChannel::getName()
{
    return m_sName;
}

QString cSampleChannel::getAlias()
{
    return m_sAlias;
}


void cSampleChannel::setRangeValidator()
{
    cStringValidator *sValidator = new cStringValidator(m_RangeNameList);
    m_pChannelRange->setValidator(sValidator);
}


void cSampleChannel::setChannelNameMetaInfo()
{
    QString s,s1,s2;

    s1 = s2 = m_sAlias;
    s1.remove(QRegExp("[1-9][0-9]?"));
    s2.remove(s1);
    //m_pChannelEntity->setValue(m_sAlias, m_pPeer);

    s = s1 + "%1" + QString(";%1;[%2]").arg(s2).arg("");
    // m_pChannelEntity->setValue(s, m_pPeer);
}


void cSampleChannel::pcbConnection()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pModule->getNetworkConfig()->m_pcbServiceConnectionInfo,
                                                                  m_pModule->getNetworkConfig()->m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pPCBClient.get(), &Zera::ProxyClient::connected, &m_readChnAliasState);

    m_pPCBInterface->setClientSmart(m_pPCBClient);
    connect(m_pPCBInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cSampleChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pPCBClient);
}


void cSampleChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAliasSample(m_sName)] = readchnaliassamplechannel;
}


void cSampleChannel::readRangelist()
{
    m_MsgNrCmdList[m_pPCBInterface->getRangeListSample(m_sName)] = readrangelistsamplechannel;
}


void cSampleChannel::activationDone()
{
    setChannelNameMetaInfo(); // we set our real name now
    setRangeValidator(); // and the list of possible ranges

    connect(m_pChannelRange, &VfModuleParameter::sigValueChanged, this, &cSampleChannel::newPLLRange);
    emit activated();
}


void cSampleChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(m_pPCBInterface.get(), 0, this, 0);
    emit deactivated();
}


void cSampleChannel::newPLLRange(QVariant rng)
{
    m_sNewRange = rng.toString();
    // m_pPllSignal->m_pParEntity->setValue(QVariant(1), m_pPeer); // we signal that we are changing pll channel
    m_MsgNrCmdList[m_pPCBInterface->setRangeSample(m_sName, m_sNewRange)] = setsamplechannelrange;
}


}
