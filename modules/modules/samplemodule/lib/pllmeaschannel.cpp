#include "pllmeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace SAMPLEMODULE
{

cPllMeasChannel::cPllMeasChannel(cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr, bool demo) :
    cBaseMeasChannel(rmsocket, pcbsocket, name, chnnr),
    m_demo(demo)
{
    m_pPCBInterface = new Zera::cPCBInterface();

    // setting up statemachine for "activating" pll meas channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cPllMeasChannel::activationContinue, &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readDspChannelState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readUnitState);
    m_readUnitState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRangelistState);
    m_readRangelistState.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, &cPllMeasChannel::activationContinue, &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_readRangeProperties3State);
    m_readRangeProperties3State.addTransition(this, &cPllMeasChannel::activationLoop, &m_readRangeProperties1State);
    m_readRangeProperties3State.addTransition(this, &cPllMeasChannel::activationContinue, &m_setSenseChannelRangeNotifierState);
    m_setSenseChannelRangeNotifierState.addTransition(this, &cPllMeasChannel::activationContinue, &m_activationDoneState);
    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readChnAliasState);
    m_activationMachine.addState(&m_readSampleRateState);
    m_activationMachine.addState(&m_readUnitState);
    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_readRangeProperties1State);
    m_activationMachine.addState(&m_readRangeProperties2State);
    m_activationMachine.addState(&m_readRangeProperties3State);
    m_activationMachine.addState((&m_setSenseChannelRangeNotifierState));
    m_activationMachine.addState(&m_activationDoneState);
    if(!m_demo)
        m_activationMachine.setInitialState(&m_rmConnectState);
    else
        m_activationMachine.setInitialState(&m_activationDoneState);
    connect(&m_rmConnectState, &QState::entered, this, &cPllMeasChannel::rmConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cPllMeasChannel::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cPllMeasChannel::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cPllMeasChannel::readResource);
    connect(&m_readResourceInfoState, &QState::entered, this, &cPllMeasChannel::readResourceInfo);
    connect(&m_pcbConnectionState, &QState::entered, this, &cPllMeasChannel::pcbConnection);
    connect(&m_readDspChannelState, &QState::entered, this, &cPllMeasChannel::readDspChannel);
    connect(&m_readChnAliasState, &QState::entered, this, &cPllMeasChannel::readChnAlias);
    connect(&m_readSampleRateState, &QState::entered, this, &cPllMeasChannel::readSampleRate);
    connect(&m_readUnitState, &QState::entered, this, &cPllMeasChannel::readUnit);
    connect(&m_readRangelistState, &QState::entered, this, &cPllMeasChannel::readRangelist);
    connect(&m_readRangeProperties1State, &QState::entered, this, &cPllMeasChannel::readRangeProperties1);
    connect(&m_readRangeProperties3State, &QState::entered, this, &cPllMeasChannel::readRangeProperties3);
    connect(&m_setSenseChannelRangeNotifierState, &QState::entered, this, &cPllMeasChannel::setSenseChannelRangeNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cPllMeasChannel::activationDone);

    // setting up statemachine for "deactivating" pll meas channel
    m_deactivationInitState.addTransition(this, &cPllMeasChannel::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    if(!m_demo)
        m_deactivationMachine.setInitialState(&m_deactivationInitState);
    else
        m_deactivationMachine.setInitialState(&m_deactivationDoneState);
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


cPllMeasChannel::~cPllMeasChannel()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
    delete m_pPCBInterface;
}


double cPllMeasChannel::getUrValue()
{
    return m_RangeInfoHash[m_sActRange].urvalue;
}


void cPllMeasChannel::generateInterface()
{
    // we don't generate any interface
}


quint32 cPllMeasChannel::setyourself4PLL(QString samplesysname)
{
    if (m_bActive)
    {
        quint32 msgnr = m_pPCBInterface->setPLLChannel(samplesysname, m_sName);
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
        case sendpllchannelrmident:
            if (reply == ack) // we only continue if resource manager acknowledges
                emit activationContinue();
            else
            {
                emit errMsg(tr(rmidentErrMSG));
                emit activationError();
            }
            break;
        case readresourcetypes:
            if ((reply == ack) && (answer.toString().contains("SENSE")))
                emit activationContinue();
            else
            {
                emit errMsg((tr(resourcetypeErrMsg)));
                emit activationError();
            }
            break;
        case readresource:
            if ((reply == ack) && (answer.toString().contains(m_sName)))
                emit activationContinue();
            else
            {
                emit errMsg((tr(resourceErrMsg)));
                emit activationError();
            }
            break;
        case readresourceinfo:
        {
            bool ok1, ok2;
            int max;
            QStringList sl;


            sl = answer.toString().split(';');
            if ((reply ==ack) && (sl.length() >= 4))
            {
                max = sl.at(0).toInt(&ok1); // fixed position
                m_sDescription = sl.at(2);
                m_nPort = sl.at(3).toInt(&ok2);

                if (ok1 && ok2 && (max == 1)) // we need one but it must not be free
                {
                    emit activationContinue();
                }

                else
                {
                    emit errMsg((tr(resourceInfoErrMsg)));
                    emit activationError();
                }
            }

            else
            {
                emit errMsg((tr(resourceInfoErrMsg)));
                emit activationError();
            }

            break;

        }
        case readdspchannel:
            if (reply == ack)
            {
                m_nDspChannel = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readdspchannelErrMsg)));
                emit activationError();
            }
            break;
        case readrange:
            if (reply == ack)
            {
                m_sActRange = answer.toString();
            }
            else
            {
                emit errMsg((tr(getRangeErrMsg)));
            }
            break;
        case readchnalias:
            if (reply == ack)
            {
                m_sAlias = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readaliasErrMsg)));
                emit activationError();
            }
            break;
        case readsamplerate:
            if (reply == ack)
            {
                m_nSampleRate = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readsamplerateErrMsg)));
                emit activationError();
            }
            break;
        case readunit:
            if (reply == ack)
            {
                m_sUnit = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readunitErrMsg)));
                emit activationError();
            }
            break;
        case readrangelist:
            if (reply == ack)
            {
                m_RangeNameList = answer.toStringList();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangelistErrMsg)));
                emit activationError();
            }
            break;
        case readrngalias:
            if (reply == ack)
            {
                ri.alias = answer.toString();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangealiasErrMsg)));
                emit activationError();
            }
            break;
        case readtype:
            if (reply == ack)
            {
                ri.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangetypeErrMsg)));
                emit activationError();
            }
            break;
        case readurvalue:
            if (reply == ack)
            {
                ri.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeurvalueErrMsg)));
                emit activationError();
            }
            break;
        case readrejection:
            if (reply == ack)
            {
                ri.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangerejectionErrMsg)));
                emit activationError();
            }
            break;
        case readovrejection:
            if (reply == ack)
            {
                ri.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeovrejectionErrMsg)));
                emit activationError();
            }
            break;
        case readisavail:
            if (reply == ack)
            {
                ri.avail = answer.toBool();
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(readrangeavailErrMsg)));
                emit activationError();
            }
            break;
        case set4PLL:
            if (reply == ack)
            {}
            else
            {
                emit errMsg((tr(setPllErrMsg)));
                emit executionError();
            }; // perhaps some error output
            emit cmdDone(msgnr);
            break;
        case setchannelrangenotifier:
            if (reply == ack)
            {
                emit activationContinue();
            }
            else
            {
                emit errMsg((tr(registerpcbnotifierErrMsg)));
                emit activationError();
            }
            break;

        }
    }
}

void cPllMeasChannel::setupDemoOperation()
{
    // Stolen from cRangeMeasChannel::setupDemoOperation()
    // Set dummy channel info
    bool isVoltagePhase = false;
    switch (m_nChannelNr)
    {
    case 1:
        m_sAlias = "UL1";
        isVoltagePhase = true;
        break;
    case 2:
        m_sAlias = "UL2";
        isVoltagePhase = true;
        break;
    case 3:
        m_sAlias = "UL3";
        isVoltagePhase = true;
        break;
    case 4:
        m_sAlias = "IL1";
        break;
    case 5:
        m_sAlias = "IL2";
        break;
    case 6:
        m_sAlias = "IL3";
        break;
    case 7:
        m_sAlias = "UAUX";
        isVoltagePhase = true;
        break;
    case 8:
        m_sAlias = "IAUX";
        break;
    }
    QVector<double> nominalRanges;
    if(isVoltagePhase) {
        m_sUnit = "V";
        nominalRanges = QVector<double>() << 480.0 << 240.0 << 120.0 << 60.0 << 0.5;
    }
    else {
        m_sUnit = "A";
        nominalRanges = QVector<double>() << 1000.0 << 100.0 << 10.0 << 1.0 << 0.1 << 0.01 << 0.001;
    }
    for(auto rangeVal : qAsConst(nominalRanges)) {
        cRangeInfo rangeInfo;
        QString unitPrefix;
        double rangeValDisplay = rangeVal;
        if(rangeVal < 1) {
            unitPrefix = "m";
            rangeValDisplay *= 1000.0;
        }
        rangeInfo.alias.setNum(int(rangeValDisplay));
        rangeInfo.alias += unitPrefix+m_sUnit;
        if(m_sActRange.isEmpty()) {
            m_sActRange = rangeInfo.alias;
        }
        rangeInfo.avail = true;
        rangeInfo.urvalue = rangeVal;
        rangeInfo.rejection = 1.0;
        rangeInfo.ovrejection = 1.25;
        // ?? name
        rangeInfo.type = 1;
        m_RangeInfoHash[rangeInfo.alias] = rangeInfo;
    }
    //setRangeListAlias();
}


void cPllMeasChannel::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort);
    m_rmConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    // todo insert timer for timeout

    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cPllMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_rmClient);
    // resource manager liste sense abfragen
    // bin ich da drin ?
    // nein -> fehler activierung
    // ja -> socket von rm besorgen
    // resource bei rm belegen
    // beim pcb proxy server interface beantragen

    // quint8 m_nDspChannel; dsp kanal erfragen
    // QString m_sAlias; kanal alias erfragen
    // eine liste aller möglichen bereichen erfragen
    // d.h. (avail = 1 und type =1
    // und von diesen dann
    // alias, urvalue, rejection und ovrejection abfragen
}


void cPllMeasChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_rmInterface.rmIdent(QString("MeasChannel%1").arg(m_nChannelNr))] = sendpllchannelrmident;
}


void cPllMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cPllMeasChannel::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cPllMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", m_sName)] = readresourceinfo;
}


void cPllMeasChannel::pcbConnection()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(m_pPCBServerSocket->m_sIP, m_nPort);
    m_pcbConnectionState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_readDspChannelState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::cPCBInterface::serverAnswer, this, &cPllMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cPllMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pPCBInterface->getDSPChannel(m_sName)] = readdspchannel;
}


void cPllMeasChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName)] = readchnalias;
}


void cPllMeasChannel::readSampleRate()
{
    m_MsgNrCmdList[m_pPCBInterface->getSampleRate()] = readsamplerate;
}


void cPllMeasChannel::readUnit()
{
    m_MsgNrCmdList[m_pPCBInterface->getUnit(m_sName)] = readunit;
}


void cPllMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pPCBInterface->getRangeList(m_sName)] = readrangelist;
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
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("sens:%1:rang?").arg(m_sName), notifierNr)] = setchannelrangenotifier;
}


void cPllMeasChannel::activationDone()
{
    if(m_demo)
        setupDemoOperation();
    QHash<QString, cRangeInfo>::iterator it = m_RangeInfoHash.begin();
    while (it != m_RangeInfoHash.end()) // we delete all unused ranges
    {
        ri = it.value();
        if (!ri.avail) // in case range is not avail
            it = m_RangeInfoHash.erase(it);
        else
            ++it;
    }

    if(!m_demo)
        readRange(); // we read the actual range once here, afterwards via notifier
    m_bActive = true;
    emit activated();
}


void cPllMeasChannel::deactivationInit()
{
    // deactivation means we have to free our resources
    // m_MsgNrCmdList[m_rmInterface.freeResource("SENSE", m_sName)] = freeresource;
    // but we didn't claim here so we continue at once
    m_bActive = false;
    emit deactivationContinue();
}


void cPllMeasChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}


void cPllMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cPllMeasChannel::readType()
{
    m_MsgNrCmdList[m_pPCBInterface->getType(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}


void cPllMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pPCBInterface->getUrvalue(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readurvalue;
}


void cPllMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pPCBInterface->getRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrejection;
}


void cPllMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pPCBInterface->getOVRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readovrejection;
}


void cPllMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pPCBInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cPllMeasChannel::rangeQueryDone()
{
    ri.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[ri.name] = ri; // for each range we append cRangeinfo per name
}


void cPllMeasChannel::readRange()
{
    m_MsgNrCmdList[m_pPCBInterface->getRange(m_sName)] = readrange;
}


}



