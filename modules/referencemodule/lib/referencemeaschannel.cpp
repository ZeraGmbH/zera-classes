#include "referencemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace REFERENCEMODULE
{

cReferenceMeasChannel::cReferenceMeasChannel(NetworkConnectionInfo rmsocket, NetworkConnectionInfo pcbsocket, VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                             QString name, quint8 chnnr) :
    cBaseMeasChannel(rmsocket, pcbsocket, tcpNetworkFactory, name, chnnr)
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    // setting up statemachine for "activating" reference meas channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readDspChannelState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readUnitState);
    m_readUnitState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readRangelistState);
    m_readRangelistState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_readRangeProperties3State);
    m_readRangeProperties3State.addTransition(this, &cReferenceMeasChannel::activationLoop, &m_readRangeProperties1State);
    m_readRangeProperties3State.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_activationDoneState);
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
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_rmConnectState);

    connect(&m_rmConnectState, &QState::entered, this, &cReferenceMeasChannel::rmConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cReferenceMeasChannel::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cReferenceMeasChannel::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cReferenceMeasChannel::readResource);
    connect(&m_readResourceInfoState, &QState::entered, this, &cReferenceMeasChannel::readResourceInfo);
    connect(&m_pcbConnectionState, &QState::entered, this, &cReferenceMeasChannel::pcbConnection);
    connect(&m_readDspChannelState, &QState::entered, this, &cReferenceMeasChannel::readDspChannel);
    connect(&m_readChnAliasState, &QState::entered, this, &cReferenceMeasChannel::readChnAlias);
    connect(&m_readSampleRateState, &QState::entered, this, &cReferenceMeasChannel::readSampleRate);
    connect(&m_readUnitState, &QState::entered, this, &cReferenceMeasChannel::readUnit);
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
    m_readRngAliasState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readTypeState);
    m_readTypeState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_readisAvailState);
    m_readisAvailState.addTransition(this, &cReferenceMeasChannel::activationContinue, &m_rangeQueryDoneState);

    m_rangeQueryMachine.addState(&m_readRngAliasState);
    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readRngAliasState);

    connect(&m_readRngAliasState, &QState::entered, this, &cReferenceMeasChannel::readRngAlias);
    connect(&m_readTypeState, &QState::entered, this, &cReferenceMeasChannel::readType);
    connect(&m_readisAvailState, &QState::entered, this, &cReferenceMeasChannel::readisAvail);
    connect(&m_rangeQueryDoneState, &QState::entered, this, &cReferenceMeasChannel::rangeQueryDone);
}


quint32 cReferenceMeasChannel::setRange(QString range)
{
    quint32 msgnr = 0;
    m_sNewRange = range;
    m_sActRange = m_sNewRange;
    msgnr = m_pcbInterface->setRange(m_sName, m_RangeInfoHash[range].name);
    m_MsgNrCmdList[msgnr] = setmeaschannelrange;
    return msgnr;
}


void cReferenceMeasChannel::generateInterface()
{
    // we have no interface
}


void cReferenceMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;
    int cmd = m_MsgNrCmdList.take(msgnr);

    switch (cmd)
    {
    case sendmeaschannelrmident:
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

            if (ok1 && ok2 && (max ==1))
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
            m_rangeInfo.alias = answer.toString();
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
            m_rangeInfo.type = answer.toInt(&ok);
            emit activationContinue();
        }
        else
        {
            emit errMsg((tr(readrangetypeErrMsg)));
            emit activationError();
        }
        break;
    case readisavail:
        if (reply == ack)
        {
            m_rangeInfo.avail = answer.toBool();
            emit activationContinue();
        }
        else
        {
            emit errMsg((tr(readrangeavailErrMsg)));
            emit activationError();
        }
        break;
    case setmeaschannelrange:
        if (reply == ack)
            m_sActRange = m_sNewRange;
        else
        {
            emit errMsg((tr(setRangeErrMsg)));
            emit executionError();
        }; // perhaps some error output
        emit cmdDone(msgnr);
        break;    
    case readgaincorrection:
        if (reply == ack)
            m_fGainCorrection = answer.toDouble(&ok);
        else
        {
            emit errMsg((tr(readGainCorrErrMsg)));
        };
        emit cmdDone(msgnr);
        break;
    case readoffsetcorrection:
        if (reply == ack)
            m_fOffsetCorrection = answer.toDouble(&ok);
        else
        {
            emit errMsg((tr(readOffsetCorrErrMsg)));
            emit executionError();
        };
        emit cmdDone(msgnr);
        break;
    case readphasecorrection:
        if (reply == ack)
            m_fPhaseCorrection = answer.toDouble(&ok);
        else
        {
            emit errMsg((tr(readPhaseCorrErrMsg)));
            emit executionError();
        };
        emit cmdDone(msgnr);
        break;
    case readmeaschannelstatus:
        if (reply == ack)
            m_nStatus = answer.toInt(&ok);
        else
        {
            emit errMsg((tr(readChannelStatusErrMsg)));
            emit executionError();
        };
        emit cmdDone(msgnr);
        break;
    case resetmeaschannelstatus:
        if (reply == ack)
            {}
        else
        {
            emit errMsg((tr(resetChannelStatusErrMsg)));
            emit executionError();
        }; // perhaps some error output
        emit cmdDone(msgnr);
        break;
    }
}

void cReferenceMeasChannel::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_resmanNetworkInfo.m_sIP,
                                                                m_resmanNetworkInfo.m_nPort,
                                                                m_tcpNetworkFactory);
    m_rmConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_rmInterface.setClientSmart(m_rmClient); //
    // todo insert timer for timeout

    connect(&m_rmInterface, &Zera::cRMInterface::serverAnswer, this, &cReferenceMeasChannel::catchInterfaceAnswer);
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


void cReferenceMeasChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_rmInterface.rmIdent(QString("ReferenceMeasChannel%1").arg(m_nChannelNr))] = sendmeaschannelrmident;
}


void cReferenceMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cReferenceMeasChannel::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cReferenceMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", m_sName)] = readresourceinfo;
}


void cReferenceMeasChannel::pcbConnection()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pcbNetworkInfo.m_sIP,
                                                                 m_nPort,
                                                                 m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbClient.get(), &Zera::ProxyClient::connected, &m_readDspChannelState);

    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cReferenceMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cReferenceMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pcbInterface->getDSPChannel(m_sName)] = readdspchannel;
}


void cReferenceMeasChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pcbInterface->getAlias(m_sName)] = readchnalias;
}


void cReferenceMeasChannel::readSampleRate()
{
    m_MsgNrCmdList[m_pcbInterface->getSampleRate()] = readsamplerate;
}


void cReferenceMeasChannel::readUnit()
{
    m_MsgNrCmdList[m_pcbInterface->getUnit(m_sName)] = readunit;
}


void cReferenceMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pcbInterface->getRangeList(m_sName)] = readrangelist;
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
    if (m_RangeQueryIt < m_RangeNameList.count()) // another range ?
        emit activationLoop();
    else
        emit activationContinue();
}


void cReferenceMeasChannel::activationDone()
{
    QHash<QString, cRangeInfoBase>::iterator it = m_RangeInfoHash.begin();
    while (it != m_RangeInfoHash.end()) // we delete all unused ranges
    {
        m_rangeInfo = it.value();
        if (!m_rangeInfo.avail || ((m_rangeInfo.type & 1) == 1)) // in case range is not avail or virtual
            it = m_RangeInfoHash.erase(it);
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
    disconnect(&m_rmInterface, 0, this, 0);
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}


void cReferenceMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pcbInterface->getAlias(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cReferenceMeasChannel::readType()
{
    m_MsgNrCmdList[m_pcbInterface->getType(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}

void cReferenceMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pcbInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cReferenceMeasChannel::rangeQueryDone()
{
    m_rangeInfo.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[m_rangeInfo.alias] = m_rangeInfo; // for each range we append cRangeinfo per alias
}

}
