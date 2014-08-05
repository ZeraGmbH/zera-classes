#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "rangemeaschannel.h"

cRangeMeasChannel::cRangeMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr)
    :cBaseMeasChannel(proxy, peer, rmsocket, pcbsocket, name, chnnr)
{
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" meas channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceState);
    m_claimResourceState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readRangelistState);
    m_readRangelistState.addTransition(this, SIGNAL(activationContinue()), &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, SIGNAL(activationContinue()), &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(&m_rangeQueryMachine, SIGNAL(finished()), &m_readRangeProperties3State);
    m_readRangeProperties3State.addTransition(this, SIGNAL(activationLoop()), &m_readRangeProperties1State);
    m_readRangeProperties3State.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);
    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_claimResourceState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readChnAliasState);
    m_activationMachine.addState(&m_readRangelistState);
    m_activationMachine.addState(&m_readRangeProperties1State);
    m_activationMachine.addState(&m_readRangeProperties2State);
    m_activationMachine.addState(&m_readRangeProperties3State);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_rmConnectState);
    connect(&m_rmConnectState, SIGNAL(entered()), SLOT(rmConnect()));
    connect(&m_IdentifyState, SIGNAL(entered()), SLOT(sendRMIdent()));
    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_claimResourceState, SIGNAL(entered()), SLOT(claimResource()));
    connect(&m_pcbConnectionState, SIGNAL(entered()), SLOT(pcbConnection()));
    connect(&m_readDspChannelState, SIGNAL(entered()), SLOT(readDspChannel()));
    connect(&m_readChnAliasState, SIGNAL(entered()), SLOT(readChnAlias()));
    connect(&m_readRangelistState, SIGNAL(entered()), SLOT(readRangelist()));
    connect(&m_readRangeProperties1State, SIGNAL(entered()), SLOT(readRangeProperties1()));
    connect(&m_readRangeProperties3State, SIGNAL(entered()), SLOT(readRangeProperties3()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);
    connect(&m_deactivationInitState, SIGNAL(entered()), SLOT(deactivationInit()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));

    // setting up statemachine for querying the meas channels ranges properties
    m_readRngAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readTypeState);
    m_readTypeState.addTransition(this, SIGNAL(activationContinue()), &m_readUrvalueState);
    m_readUrvalueState.addTransition(this, SIGNAL(activationContinue()), &m_readRejectionState);
    m_readRejectionState.addTransition(this, SIGNAL(activationContinue()), &m_readOVRejectionState);
    m_readOVRejectionState.addTransition(this, SIGNAL(activationContinue()), &m_readisAvailState);
    m_readisAvailState.addTransition(this, SIGNAL(activationContinue()), &m_rangeQueryDoneState);


    m_rangeQueryMachine.addState(&m_readRngAliasState);
    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readUrvalueState);
    m_rangeQueryMachine.addState(&m_readRejectionState);
    m_rangeQueryMachine.addState(&m_readOVRejectionState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readRngAliasState);

    connect(&m_readRngAliasState, SIGNAL(entered()), SLOT(readRngAlias()));
    connect(&m_readTypeState, SIGNAL(entered()), SLOT(readType()));
    connect(&m_readUrvalueState, SIGNAL(entered()), SLOT(readUrvalue()));
    connect(&m_readRejectionState, SIGNAL(entered()), SLOT(readRejection()));
    connect(&m_readOVRejectionState, SIGNAL(entered()), SLOT(readOVRejection()));
    connect(&m_readisAvailState, SIGNAL(entered()), SLOT(readisAvail()));
    connect(&m_rangeQueryDoneState, SIGNAL(entered()), SLOT(rangeQueryDone()));

}


cRangeMeasChannel::~cRangeMeasChannel()
{
    m_pProxy->releaseConnection(m_pRMClient);
    m_pProxy->releaseConnection(m_pPCBClient);
    delete m_pRMInterface;
    delete m_pPCBInterface;
}


quint32 cRangeMeasChannel::setRange(QString range)
{
    m_sNewRange = range;
    m_sActRange = m_sNewRange;
    quint32 msgnr = m_pPCBInterface->setRange(m_sName, m_RangeInfoHash[range].name);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::setrange;
    return msgnr;
}


quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    quint32 msgnr = m_pPCBInterface->getGainCorrection(m_sName, m_sActRange, amplitude);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::readgaincorrection;
    return msgnr;
}


quint32 cRangeMeasChannel::readOffsetCorrection(double amplitude)
{
    quint32 msgnr = m_pPCBInterface->getOffsetCorrection(m_sName, m_sActRange, amplitude);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::readoffsetcorrection;
    return msgnr;
}


quint32 cRangeMeasChannel::readStatus()
{
    quint32 msgnr = m_pPCBInterface->getStatus(m_sName);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::readstatus;
    return msgnr;
}


quint32 cRangeMeasChannel::resetStatus()
{
    quint32 msgnr = m_pPCBInterface->resetStatus(m_sName);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::resetstatus;
    return msgnr;
}


quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    quint32 msgnr = m_pPCBInterface->getPhaseCorrection(m_sName, m_sActRange, frequency);
    m_MsgNrCmdList[msgnr] = RANGEMEASCHANNEL::readphasecorrection;
    return msgnr;
}


bool cRangeMeasChannel::isPossibleRange(QString range, double ampl)
{
    cRangeInfo ri;
    ri = m_RangeInfoHash[range];
    return ((ri.urvalue * ri.ovrejection *sqrt2 / ri.rejection) >= ampl);
    // return (m_RangeInfoHash[range].urvalue * sqrt2 >= ampl);
}


bool cRangeMeasChannel::isPossibleRange(QString range)
{
    return m_RangeInfoHash.contains(range);
}


bool cRangeMeasChannel::isOverload(double ampl)
{
    cRangeInfo ri;
    ri = m_RangeInfoHash[m_sActRange];
    // qDebug() << QString("Ampl: %1; Rng: %2").arg(ampl).arg(ri.alias);
    return ((ri.urvalue * ri.ovrejection * sqrt2 /ri.rejection) < ampl);
}


QString cRangeMeasChannel::getOptRange(double ampl)
{
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = 1e32;
    double newUrvalue;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++)
    {
        cRangeInfo ri = riList.at(i);
        newUrvalue = ri.urvalue;
        if (((newUrvalue * sqrt2 *ri.ovrejection /ri.rejection ) >= ampl) && (newUrvalue < newAmpl))
        {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    if (p > -1)
        return riList.at(p).alias;
    else
        return getMaxRange(); // we return maximum range in case of overload condtion
}


QString cRangeMeasChannel::getMaxRange()
{
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = 0;
    double newUrvalue;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++)
    {
        newUrvalue = riList.at(i).urvalue;
        if (newUrvalue > newAmpl)
        {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    return riList.at(p).alias;
}


double cRangeMeasChannel::getGainCorrection()
{
    return m_fGainCorrection;
}


double cRangeMeasChannel::getPhaseCorrection()
{
    return m_fPhaseCorrection;
}


double cRangeMeasChannel::getOffsetCorrection()
{
    return m_fOffsetCorrection;
}


bool cRangeMeasChannel::isHWOverload()
{
    return ((m_nStatus & 1) > 0);
}


double cRangeMeasChannel::getUrValue(QString range)
{
    return m_RangeInfoHash[range].urvalue;
}


double cRangeMeasChannel::getUrValue()
{
    return m_RangeInfoHash[m_sActRange].urvalue;
}


double cRangeMeasChannel::getRejection(QString range)
{
    return m_RangeInfoHash[range].rejection;
}


double cRangeMeasChannel::getRejection()
{
    return m_RangeInfoHash[m_sActRange].rejection;
}


double cRangeMeasChannel::getOVRRejection(QString range)
{
    return m_RangeInfoHash[range].ovrejection;
}


double cRangeMeasChannel::getOVRRejection()
{
    return m_RangeInfoHash[m_sActRange].ovrejection;
}


double cRangeMeasChannel::getMaxRecjection()
{
   QString s = getMaxRange();
   return (getUrValue(s) * getOVRRejection(s) / getRejection(s));
}


void cRangeMeasChannel::generateInterface()
{
    QString s;

    m_pChannelEntity = m_pPeer->dataAdd(QString("INF_Channel%1Name").arg(m_nChannelNr)); // here is the actual range
    m_pChannelEntity->modifiersAdd(VeinEntity::MOD_READONLY);
    m_pChannelEntity->setValue(s = "Unknown", m_pPeer);

    m_pChannelRangeListEntity = m_pPeer->dataAdd(QString("INF_Channel%1RangeList").arg(m_nChannelNr)); // list of possible ranges
    m_pChannelRangeListEntity->modifiersAdd(VeinEntity::MOD_READONLY);
    m_pChannelRangeListEntity->setValue(s = "Unknown", m_pPeer);
}


void cRangeMeasChannel::deleteInterface()
{
    m_pPeer->dataRemove(m_pChannelEntity);
    m_pPeer->dataRemove(m_pChannelRangeListEntity);
}


void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;
    int cmd = m_MsgNrCmdList.take(msgnr);

    switch (cmd)
    {
    case RANGEMEASCHANNEL::sendrmident:
        if (reply == ack) // we only continue if resource manager acknowledges
            emit activationContinue();
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readresourcetypes:
        if ((reply == ack) && (answer.toString().contains("SENSE")))
            emit activationContinue();
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readresource:
        if ((reply == ack) && (answer.toString().contains(m_sName)))
            emit activationContinue();
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readresourceinfo:
    {
        bool ok1, ok2, ok3;
        int max, free;
        QStringList sl;


        sl = answer.toString().split(';');
        if ((reply ==ack) && (sl.length() >= 4))
        {
            max = sl.at(0).toInt(&ok1); // fixed position
            free = sl.at(1).toInt(&ok2);
            m_sDescription = sl.at(2);
            m_nPort = sl.at(3).toInt(&ok3);

            if (ok1 && ok2 && ok3 && ((max == free) == 1))
            {
                emit activationContinue();
            }

            else
                emit activationError();
        }

        else
            emit activationError();

        break;

    }
    case RANGEMEASCHANNEL::claimresource:
        if (reply == ack)
            emit activationContinue();
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::freeresource:
        if (reply == ack || reply == nack) // we accept nack here also
            emit deactivationContinue(); // maybe that resource was deleted by server and then it is no more set
        else
            emit deactivationError();
        break;
    case RANGEMEASCHANNEL::readdspchannel:
        if (reply == ack)
        {
            m_nDspChannel = answer.toInt(&ok);
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readchnalias:
        if (reply == ack)
        {
            m_sAlias = answer.toString();
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readrangelist:
        if (reply == ack)
        {
            m_RangeNameList = answer.toStringList();
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readrngalias:
        if (reply == ack)
        {
            ri.alias = answer.toString();
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readtype:
        if (reply == ack)
        {
            ri.type = answer.toInt(&ok);
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readurvalue:
        if (reply == ack)
        {
            ri.urvalue = answer.toDouble(&ok);
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readrejection:
        if (reply == ack)
        {
            ri.rejection = answer.toDouble(&ok);
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readovrejection:
        if (reply == ack)
        {
            ri.ovrejection = answer.toDouble(&ok);
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::readisavail:
        if (reply == ack)
        {
            ri.avail = answer.toBool();
            emit activationContinue();
        }
        else
            emit activationError();
        break;
    case RANGEMEASCHANNEL::setrange:
        if (reply == ack)
            m_sActRange = m_sNewRange;
        else {}; // perhaps some error output
        emit cmdDone(msgnr);
        break;    
    case RANGEMEASCHANNEL::readgaincorrection:
        if (reply == ack)
            m_fGainCorrection = answer.toDouble(&ok);
        else {};
        emit cmdDone(msgnr);
        break;
    case RANGEMEASCHANNEL::readoffsetcorrection:
        if (reply == ack)
            m_fOffsetCorrection = answer.toDouble(&ok);
        else {};
        emit cmdDone(msgnr);
        break;
    case RANGEMEASCHANNEL::readphasecorrection:
        if (reply == ack)
            m_fPhaseCorrection = answer.toDouble(&ok);
        else {};
        emit cmdDone(msgnr);
        break;
    case RANGEMEASCHANNEL::readstatus:
        if (reply == ack)
            m_nStatus = answer.toInt(&ok);
        else {};
        emit cmdDone(msgnr);
        break;
    case RANGEMEASCHANNEL::resetstatus:
        if (reply == ack)
            {}
        else {}; // perhaps some error output
        emit cmdDone(msgnr);
        break;
    }
}


void cRangeMeasChannel::setRangeListEntity()
{
    QString s;
    QList<cRangeInfo> riList = m_RangeInfoHash.values();

    // we sort the range alias according to upper range values
    for (int i = 0; i < riList.count()-1; i++)
        for (int j = i; j < riList.count(); j++)
            if (riList.at(i).urvalue < riList.at(j).urvalue)
                riList.swap(i, j);

    s = riList.at(0).alias;
    for (int i = 1; i < riList.count(); i++)
        s = s + ";" + riList.at(i).alias;

    m_pChannelRangeListEntity->setValue(s, m_pPeer);
}


void cRangeMeasChannel::setChannelEntity()
{
    m_pChannelEntity->setValue(m_sAlias, m_pPeer);
}


void cRangeMeasChannel::rmConnect()
{
    // we instantiate a working resource manager interface first
    // so first we try to get a connection to resource manager over proxy
    m_pRMClient = m_pProxy->getConnection(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort);
    m_rmConnectState.addTransition(m_pRMClient, SIGNAL(connected()), &m_IdentifyState);
    // and then we set connection resource manager interface's connection
    m_pRMInterface->setClient(m_pRMClient); //
    // todo insert timer for timeout

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));

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


void cRangeMeasChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_pRMInterface->rmIdent(QString("MeasChannel%1").arg(m_nChannelNr))] = RANGEMEASCHANNEL::sendrmident;
}


void cRangeMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = RANGEMEASCHANNEL::readresourcetypes;
}


void cRangeMeasChannel::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = RANGEMEASCHANNEL::readresource;
}


void cRangeMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", m_sName)] = RANGEMEASCHANNEL::readresourceinfo;
}


void cRangeMeasChannel::claimResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SENSE", m_sName, 1)] = RANGEMEASCHANNEL::claimresource;
}


void cRangeMeasChannel::pcbConnection()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_nPort);
    m_pcbConnectionState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_readDspChannelState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
}


void cRangeMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pPCBInterface->getDSPChannel(m_sName)] = RANGEMEASCHANNEL::readdspchannel;
}


void cRangeMeasChannel::readChnAlias()
{
   m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName)] = RANGEMEASCHANNEL::readchnalias;
}


void cRangeMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pPCBInterface->getRangeList(m_sName)] = RANGEMEASCHANNEL::readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}


void cRangeMeasChannel::readRangeProperties1()
{
    m_rangeQueryMachine.start(); // yes, fill it with information
    emit activationContinue();
}


void cRangeMeasChannel::readRangeProperties3()
{
    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_RangeNameList.count()) // another range ?
        emit activationLoop();
    else
        emit activationContinue();
}


void cRangeMeasChannel::activationDone()
{
    QHash<QString, cRangeInfo>::iterator it = m_RangeInfoHash.begin();
    while (it != m_RangeInfoHash.end()) // we delete all unused ranges
    {
        ri = it.value();
        if (!ri.avail || ((ri.type & 1) == 1)) // in case range is not avail or virtual
            it = m_RangeInfoHash.erase(it);
        else
            ++it;
    }

    setChannelEntity(); // we set our real name now
    setRangeListEntity(); // and the list of possible ranges

    emit activated();
}


void cRangeMeasChannel::deactivationInit()
{
    // deactivation means we have to free our resources
    m_MsgNrCmdList[m_pRMInterface->freeResource("SENSE", m_sName)] = RANGEMEASCHANNEL::freeresource;
}


void cRangeMeasChannel::deactivationDone()
{
    // and disconnect for our servers afterwards
    disconnect(m_pRMInterface, 0, this, 0);
    disconnect(m_pPCBInterface, 0, this, 0);
    emit deactivated();
}


void cRangeMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readrngalias;
}


void cRangeMeasChannel::readType()
{
    m_MsgNrCmdList[m_pPCBInterface->getType(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readtype;
}


void cRangeMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pPCBInterface->getUrvalue(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readurvalue;
}


void cRangeMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pPCBInterface->getRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readrejection;
}


void cRangeMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pPCBInterface->getOVRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readovrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pPCBInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = RANGEMEASCHANNEL::readisavail;
}


void cRangeMeasChannel::rangeQueryDone()
{
    ri.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[ri.alias] = ri; // for each range we append cRangeinfo per alias
}


