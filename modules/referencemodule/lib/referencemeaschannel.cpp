#include "referencemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace REFERENCEMODULE
{

enum rangemeaschannelCmds
{
    readrangelist,

    readtype,
    readisavail,

    setmeaschannelrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection,
};

cReferenceMeasChannel::cReferenceMeasChannel(const ChannelRangeObserver::ChannelPtr &channelObserver,
                                             const NetworkConnectionInfo &pcbsocket,
                                             const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                             const QString &moduleName) :
    cBaseMeasChannel(pcbsocket, tcpNetworkFactory, channelObserver,
                     QString("%1/cReferenceMeasChannel/%2").arg(moduleName, channelObserver->getMName()))
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    // setting up statemachine for "activating" reference meas channel
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readRangelistState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_readRangeProperties3State);
    m_readRangeProperties3State.addTransition(this, &cReferenceMeasChannel::activationLoop, &m_readRangeProperties1State);
    m_readRangeProperties3State.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_readRangeProperties1State);
    m_activationMachine.addState(&m_readRangeProperties2State);
    m_activationMachine.addState(&m_readRangeProperties3State);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_pcbConnectionState);

    connect(&m_pcbConnectionState, &QState::entered, this, &cReferenceMeasChannel::pcbConnection);
    connect(&m_readRangelistState, &QState::entered, this, &cReferenceMeasChannel::readRangelist);
    connect(&m_readRangeProperties1State, &QState::entered, this, &cReferenceMeasChannel::readRangeProperties1);
    connect(&m_readRangeProperties3State, &QState::entered, this, &cReferenceMeasChannel::readRangeProperties3);
    connect(&m_activationDoneState, &QState::entered, this, &cReferenceMeasChannel::activationDone);

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, &cReferenceMeasChannel::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_deactivationInitState);

    connect(&m_deactivationInitState, &QState::entered, this, &cReferenceMeasChannel::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cReferenceMeasChannel::deactivationDone);

    // setting up statemachine for querying the meas channels ranges properties
    m_readTypeState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readisAvailState);
    m_readisAvailState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_rangeQueryDoneState);

    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readTypeState);

    connect(&m_readTypeState, &QState::entered, this, &cReferenceMeasChannel::readType);
    connect(&m_readisAvailState, &QState::entered, this, &cReferenceMeasChannel::readisAvail);
    connect(&m_rangeQueryDoneState, &QState::entered, this, &cReferenceMeasChannel::rangeQueryDone);
}

quint32 cReferenceMeasChannel::setRange(QString range)
{
    m_nextRangeAlias = range;
    m_actRangeAlias = m_nextRangeAlias;
    quint32 msgnr = m_pcbInterface->setRange(getMName(), m_rangeAliasToChannelData[range].name);
    m_MsgNrCmdList[msgnr] = setmeaschannelrange;
    return msgnr;
}

void cReferenceMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case readrangelist:
        if (reply == ack)
        {
            m_allRangeNamesOrderedByServer = answer.toStringList();
            emit activationContinue();
        }
        else
            notifyError(readrangelistErrMsg);
        break;

    case readtype:
        if (reply == ack) {
            m_rangeInfo.type = answer.toInt();
            emit activationContinue();
        }
        else
            notifyError(readrangetypeErrMsg);
        break;
    case readisavail:
        if (reply == ack) {
            m_rangeInfo.avail = answer.toBool();
            emit activationContinue();
        }
        else
            notifyError(readrangeavailErrMsg);
        break;
    case setmeaschannelrange:
        if (reply == ack)
            m_actRangeAlias = m_nextRangeAlias;
        else
            notifyError(setRangeErrMsg);
        emit cmdDone(msgnr);
        break;    
    case readgaincorrection:
        if (reply == ack)
            m_fGainCorrection = answer.toDouble();
        else
            notifyError(readGainCorrErrMsg);
        emit cmdDone(msgnr);
        break;
    case readoffsetcorrection:
        if (reply == ack)
            m_fOffsetCorrection = answer.toDouble();
        else
            notifyError(readOffsetCorrErrMsg);
        emit cmdDone(msgnr);
        break;
    case readphasecorrection:
        if (reply == ack)
            m_fPhaseCorrection = answer.toDouble();
        else
            notifyError(readPhaseCorrErrMsg);
        emit cmdDone(msgnr);
        break;
    }
}

void cReferenceMeasChannel::pcbConnection()
{
    m_pcbInterface->setClientSuperSmart(m_pcbNetworkInfo, m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbInterface->getClientSmart().get(), &Zera::ProxyClient::connected, &m_readRangelistState);

    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cReferenceMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbInterface->getClientSmart());
}


void cReferenceMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pcbInterface->getRangeList(getMName())] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}


void cReferenceMeasChannel::readRangeProperties1()
{
    m_rangeQueryMachine.start(); // yes, fill it with information
    emit activationContinue();
}


void cReferenceMeasChannel::readRangeProperties3()
{
    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_allRangeNamesOrderedByServer.count()) // another range ?
        emit activationLoop();
    else
        emit activationContinue();
}


void cReferenceMeasChannel::activationDone()
{
    QHash<QString, cRangeInfoBase>::iterator it = m_rangeAliasToChannelData.begin();
    while (it != m_rangeAliasToChannelData.end()) // we delete all unused ranges
    {
        m_rangeInfo = it.value();
        if (!m_rangeInfo.avail || ((m_rangeInfo.type & 1) == 1)) // in case range is not avail or virtual
            it = m_rangeAliasToChannelData.erase(it);
        else
            ++it;
    }

    emit activated();
}

void cReferenceMeasChannel::deactivationInit()
{
    emit deactivationContinue();
}

void cReferenceMeasChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

void cReferenceMeasChannel::readType()
{
    m_MsgNrCmdList[m_pcbInterface->getType(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readtype;
}

void cReferenceMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pcbInterface->isAvail(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readisavail;
}

void cReferenceMeasChannel::rangeQueryDone()
{
    m_rangeInfo.name = m_allRangeNamesOrderedByServer.at(m_RangeQueryIt);
    m_rangeAliasToChannelData[ChannelRangeObserver::Range::getAliasFromName(m_rangeInfo.name)] = m_rangeInfo;
}

}
