#include "pllmeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace SAMPLEMODULE
{

cPllMeasChannel::cPllMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                                 NetworkConnectionInfo pcbsocket,
                                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory, QString moduleName) :
    cBaseMeasChannel(pcbsocket, tcpNetworkFactory, channelObserver,
                       QString("%1/PllMeasChannel/%2").arg(moduleName, channelObserver->getMName()))
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    // setting up statemachine for "activating" pll meas channel
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readRangelistState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_readRangeProperties3State);
    m_readRangeProperties3State.addTransition(this, &cPllMeasChannel::activationLoop, &m_readRangeProperties1State);
    m_readRangeProperties3State.addTransition(this, &cPllMeasChannel::activationContinue, &m_setSenseChannelRangeNotifierState);
    m_setSenseChannelRangeNotifierState.addTransition(this, &cPllMeasChannel::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_readRangeProperties1State);
    m_activationMachine.addState(&m_readRangeProperties2State);
    m_activationMachine.addState(&m_readRangeProperties3State);
    m_activationMachine.addState((&m_setSenseChannelRangeNotifierState));
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_pcbConnectionState);

    connect(&m_pcbConnectionState, &QState::entered, this, &cPllMeasChannel::pcbConnection);
    connect(&m_readRangelistState, &QState::entered, this, &cPllMeasChannel::readRangelist);
    connect(&m_readRangeProperties1State, &QState::entered, this, &cPllMeasChannel::readRangeProperties1);
    connect(&m_readRangeProperties3State, &QState::entered, this, &cPllMeasChannel::readRangeProperties3);
    connect(&m_setSenseChannelRangeNotifierState, &QState::entered, this, &cPllMeasChannel::setSenseChannelRangeNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cPllMeasChannel::activationDone);

    // setting up statemachine for "deactivating" pll meas channel
    m_deactivationInitState.addTransition(this, &cPllMeasChannel::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    m_deactivationMachine.setInitialState(&m_deactivationInitState);

    connect(&m_deactivationInitState, &QState::entered, this, &cPllMeasChannel::deactivationInit);
    connect(&m_deactivationDoneState, &QState::entered, this, &cPllMeasChannel::deactivationDone);

    // setting up statemachine for querying the pll meas channels ranges properties
    m_readRngAliasState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readTypeState);
    m_readTypeState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readUrvalueState);
    m_readUrvalueState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRejectionState);
    m_readRejectionState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readOVRejectionState);
    m_readOVRejectionState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readisAvailState);
    m_readisAvailState.addTransition(this, &cPllMeasChannel::activationContinue, &m_rangeQueryDoneState);


    m_rangeQueryMachine.addState(&m_readRngAliasState);
    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readUrvalueState);
    m_rangeQueryMachine.addState(&m_readRejectionState);
    m_rangeQueryMachine.addState(&m_readOVRejectionState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readRngAliasState);

    connect(&m_readRngAliasState, &QState::entered, this, &cPllMeasChannel::readRngAlias);
    connect(&m_readTypeState, &QState::entered, this, &cPllMeasChannel::readType);
    connect(&m_readUrvalueState, &QState::entered, this, &cPllMeasChannel::readUrvalue);
    connect(&m_readRejectionState, &QState::entered, this, &cPllMeasChannel::readRejection);
    connect(&m_readOVRejectionState, &QState::entered, this, &cPllMeasChannel::readOVRejection);
    connect(&m_readisAvailState, &QState::entered, this, &cPllMeasChannel::readisAvail);
    connect(&m_rangeQueryDoneState, &QState::entered, this, &cPllMeasChannel::rangeQueryDone);

}


double cPllMeasChannel::getUrValue()
{
    return m_RangeInfoHash[m_sActRange].urvalue;
}


void cPllMeasChannel::generateVeinInterface()
{
    // we don't generate any interface
}


quint32 cPllMeasChannel::setyourself4PLL(QString samplesysname)
{
    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->setPLLChannel(samplesysname, getMName());
        m_MsgNrCmdList[msgnr] = set4PLL;
        return msgnr;
    }
    else
        return 1;
}

quint32 cPllMeasChannel::setPLLMode(QString samplesysname, QString mode)
{
    if (m_bActive)
    {
        quint32 msgnr = m_pcbInterface->setPLLChannel(samplesysname, mode);
        m_MsgNrCmdList[msgnr] = set4PLL;
        return msgnr;
    }
    else
        return 1;
}


void cPllMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case notifierNr:
            // if we got notification for new range we read that information
            readRange();
            break;
        }
    }
    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);

        switch (cmd)
        {
        case readrange:
            if (reply == ack)
                m_sActRange = answer.toString();
            else
                notifyError(getRangeErrMsg);
            break;
        case readrangelist:
            if (reply == ack) {
                m_RangeNameList = answer.toStringList();
                emit activationContinue();
            }
            else
                notifyError(readrangelistErrMsg);
            break;
        case readrngalias:
            if (reply == ack) {
                ri.alias = answer.toString();
                emit activationContinue();
            }
            else
                notifyError(readrangealiasErrMsg);
            break;
        case readtype:
            if (reply == ack) {
                ri.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangetypeErrMsg);
            break;
        case readurvalue:
            if (reply == ack) {
                ri.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeurvalueErrMsg);
            break;
        case readrejection:
            if (reply == ack) {
                ri.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangerejectionErrMsg);
            break;
        case readovrejection:
            if (reply == ack) {
                ri.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeovrejectionErrMsg);
            break;
        case readisavail:
            if (reply == ack) {
                ri.avail = answer.toBool();
                emit activationContinue();
            }
            else
                notifyError(readrangeavailErrMsg);
            break;
        case set4PLL:
            if (reply == ack) {}
            else
                notifyError(setPllErrMsg);
            emit cmdDone(msgnr);
            break;
        case setchannelrangenotifier:
            if (reply == ack)
                emit activationContinue();
            else
                notifyError(registerpcbnotifierErrMsg);
            break;
        }
    }
}

void cPllMeasChannel::pcbConnection()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pcbNetworkInfo.m_sIP,
                                                                 m_pcbNetworkInfo.m_nPort,
                                                                 m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbClient.get(), &Zera::ProxyClient::connected, &m_readRangelistState);

    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cPllMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cPllMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pcbInterface->getRangeList(getMName())] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}


void cPllMeasChannel::readRangeProperties1()
{
    m_rangeQueryMachine.start(); // yes, fill it with information
    emit activationContinue();
}


void cPllMeasChannel::readRangeProperties3()
{
    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_RangeNameList.count()) // another range ?
        emit activationLoop();
    else
        emit activationContinue();
}


void cPllMeasChannel::setSenseChannelRangeNotifier()
{
    m_MsgNrCmdList[m_pcbInterface->registerNotifier(QString("sens:%1:rang?").arg(getMName()), notifierNr)] = setchannelrangenotifier;
}


void cPllMeasChannel::activationDone()
{
    QHash<QString, cRangeInfoWithConstantValues>::iterator it = m_RangeInfoHash.begin();
    while (it != m_RangeInfoHash.end()) // we delete all unused ranges
    {
        ri = it.value();
        if (!ri.avail) // in case range is not avail
            it = m_RangeInfoHash.erase(it);
        else
            ++it;
    }

    readRange(); // we read the actual range once here, afterwards via notifier
    m_bActive = true;
    emit activated();
}


void cPllMeasChannel::deactivationInit()
{
    m_bActive = false;
    emit deactivationContinue();
}


void cPllMeasChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}


void cPllMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pcbInterface->getAlias(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cPllMeasChannel::readType()
{
    m_MsgNrCmdList[m_pcbInterface->getType(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}


void cPllMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pcbInterface->getUrvalue(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readurvalue;
}


void cPllMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pcbInterface->getRejection(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readrejection;
}


void cPllMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pcbInterface->getOVRejection(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readovrejection;
}


void cPllMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pcbInterface->isAvail(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cPllMeasChannel::rangeQueryDone()
{
    ri.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[ri.name] = ri; // for each range we append cRangeinfo per name
}


void cPllMeasChannel::readRange()
{
    m_MsgNrCmdList[m_pcbInterface->getRange(getMName())] = readrange;
}


}

