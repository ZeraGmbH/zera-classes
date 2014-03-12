#include <rminterface.h>
#include <pcbinterface.h>
#include <proxy.h>
#include <veinpeer.h>
#include <veinentity.h>

#include "rangemeaschannel.h"

cRangeMeasChannel::cRangeMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr)
    :cBaseMeasChannel(proxy, peer, rmsocket, pcbsocket, name, chnnr)
{
    generateInterface();

    // setting up statemachine for "activating" meas channel
    m_readResourceTypesState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceState);
    m_readResourceState.addTransition(this, SIGNAL(activationContinue()), &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, SIGNAL(activationContinue()), &m_claimResourceState);
    m_claimResourceState.addTransition(this, SIGNAL(activationContinue()), &m_pcbConnectionState);
    m_pcbConnectionState.addTransition(this, SIGNAL(activationContinue()), &m_readDspChannelState);
    m_readDspChannelState.addTransition(this, SIGNAL(activationContinue()), &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, SIGNAL(activationContinue()), &m_readRangelistState);
    m_readRangelistState.addTransition(this, SIGNAL(activationContinue()), &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(&m_rangeQueryMachine, SIGNAL(finished()), &m_readRangeProperties2State);
    m_readRangeProperties2State.addTransition(this, SIGNAL(activationContinue()), &m_readRangeProperties1State);
    m_readRangeProperties1State.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

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
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_readResourceTypesState);

    connect(&m_readResourceTypesState, SIGNAL(entered()), SLOT(readResourceTypes()));
    connect(&m_readResourceState, SIGNAL(entered()), SLOT(readResource()));
    connect(&m_readResourceInfoState, SIGNAL(entered()), SLOT(readResourceInfo()));
    connect(&m_claimResourceState, SIGNAL(entered()), SLOT(claimResource()));
    connect(&m_pcbConnectionState, SIGNAL(entered()), SLOT(pcbConnection()));
    connect(&m_readDspChannelState, SIGNAL(entered()), SLOT(getDspChannel()));
    connect(&m_readChnAliasState, SIGNAL(entered()), SLOT(readchnAlias()));
    connect(&m_readRangelistState, SIGNAL(entered()), SLOT(readRangelist()));
    connect(&m_readRangeProperties1State, SIGNAL(entered()), SLOT(readRangeProperties1()));
    connect(&m_readRangeProperties2State, SIGNAL(entered()), SLOT(readRangeProperties2()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

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
    delete m_pRMInterface;
    deleteInterface();
}


quint32 cRangeMeasChannel::setRange(QString range)
{
    m_sNewRange = range;
    quint32 msgnr = m_pPCBInterface->setRange(m_sName, m_RangeInfoHash[range].name);
    m_MsgNrCmdList[msgnr] = setrange;
    return msgnr;
}


quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    quint32 msgnr = m_pPCBInterface->getGainCorrection(m_sName, m_sActRange, amplitude);
    m_MsgNrCmdList[msgnr] = readgaincorrection;
    return msgnr;
}


quint32 cRangeMeasChannel::readOffsetCorrection(double amplitude)
{
    quint32 msgnr = m_pPCBInterface->getOffsetCorrection(m_sName, m_sActRange, amplitude);
    m_MsgNrCmdList[msgnr] = readoffsetcorrection;
    return msgnr;
}


quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    quint32 msgnr = m_pPCBInterface->getPhaseCorrection(m_sName, m_sActRange, frequency);
    m_MsgNrCmdList[msgnr] = readphasecorrection;
    return msgnr;
}


bool cRangeMeasChannel::isPossibleRange(QString range, double ampl)
{
    return (m_RangeInfoHash[range].urvalue * sqrt2 >= ampl);
}


bool cRangeMeasChannel::isOverload(double ampl)
{
    cRangeInfo ri;
    ri = m_RangeInfoHash[m_sActRange];
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
        newUrvalue = riList.at(i).urvalue;
        if ((newUrvalue * sqrt2 >= ampl) && (newUrvalue < newAmpl))
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


double cRangeMeasChannel::getUrValue(QString range)
{
    return m_RangeInfoHash[range].urvalue;
}


double cRangeMeasChannel::getUrValue()
{
    return m_RangeInfoHash[m_sActRange].urvalue;
}


double cRangeMeasChannel::getRejection()
{
    return m_RangeInfoHash[m_sActRange].rejection;
}


void cRangeMeasChannel::activate()
{
    // mal schaun was aktivieren hier für eine bedeutung bekommt

    // we instantiate a working resource manager interface first
    m_pRMInterface = new Zera::Server::cRMInterface();
    m_pRMInterface->setClient(m_pProxy->getConnection(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort));

    connect(m_pRMInterface, SIGNAL(serverAnswer(quint32, QVariant)), this, SLOT(catchInterfaceAnswer(quint32,QVariant)));
    m_activationMachine.start(); // starting statemachine

    // resource manager liste sense abfragen
    // bin ich da drin ?
    // nein -> fehler activierung
    // ja -> socket von rm besorgen
    // beim pcb proxy server interface beantragen

    // quint8 m_nDspChannel; dsp kanal erfragen
    // QString m_sAlias; kanal alias erfragen
    // eine liste aller möglichen bereichen erfragen
    // d.h. (avail = 1 und type =1
    // und von diesen dann
    // alias, urvalue, rejection und ovrejection abfragen
}


void cRangeMeasChannel::deactivate()
{
    // gilt auch fürs deaktivieren
}


void cRangeMeasChannel::generateInterface()
{
    QStringList sl;
    QString s;

    m_pChannelEntity = m_pPeer->dataAdd(QString("Channel%1").arg(m_nChannelNr));
    m_pChannelEntity->modifiersAdd(VeinEntity::MOD_READONLY);
    m_pChannelEntity->setValue(QVariant(s = "Unknown"));

    m_pChannelRangeListEntity = m_pPeer->dataAdd(QString("Channel%1RangeList").arg(m_nChannelNr));
    m_pChannelRangeListEntity->modifiersAdd(VeinEntity::MOD_READONLY);
    m_pChannelRangeListEntity->setValue(QVariant(sl << "Unknown"));
}


void cRangeMeasChannel::deleteInterface()
{
    m_pPeer->dataRemove(m_pChannelEntity);
    m_pPeer->dataRemove(m_pChannelRangeListEntity);
}


void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, QVariant answer)
{
    bool ok;
    int cmd = m_MsgNrCmdList.take(msgnr);
    switch (cmd)
    {
    case readresourcetypes:
        if (answer.toString().contains("SENSE"))
            emit activationContinue();
        else
            emit activationError();
        break;
    case readresource:
        if (answer.toString().contains(m_sName))
            emit activationContinue();
        else
            emit activationError();
        break;
    case readresourceinfo:
    {
        bool ok1, ok2, ok3;
        int max, free;
        QStringList sl;

        sl = answer.toString().split(';');
        if (sl.length() == 4)
        {
            max = sl.at(0).toInt(&ok1); // fixed position
            free = sl.at(1).toInt(&ok2);
            m_nPort = sl.at(2).toInt(&ok3);
            m_sDescription = sl.at(3);

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
    case claimresource:
        if (answer.toInt(&ok) == ack)
            emit activationContinue();
        else
            emit activationError();
        break;
    case readdspchannel:
        m_nDspChannel = answer.toInt(&ok);
        emit activationContinue();
        break;
    case readchnalias:
        m_sAlias = answer.toString();
        emit activationContinue();
        break;
    case readrangelist:
        m_RangeNameList = answer.toStringList();
        emit activationContinue();
        break;
    case readrngalias:
        ri.alias = answer.toString();
        emit activationContinue();
        break;
    case readtype:
        ri.type = answer.toInt(&ok);
        emit activationContinue();
        break;
    case readurvalue:
        ri.urvalue = answer.toDouble(&ok);
        emit activationContinue();
        break;
    case readrejection:
        ri.rejection = answer.toDouble(&ok);
        emit activationContinue();
        break;
    case readovrejection:
        ri.ovrejection = answer.toDouble(&ok);
        emit activationContinue();
        break;
    case readisavail:
        ri.avail = answer.toBool();
        emit activationContinue();
        break;
    case setrange:
        m_sActRange = m_sNewRange;
        emit cmdDone(msgnr);
        break;
    case readgaincorrection:
        m_fGainCorrection = answer.toDouble(&ok);
        emit cmdDone(msgnr);
        break;
    case readoffsetcorrection:
        m_fOffsetCorrection = answer.toDouble(&ok);
        emit cmdDone(msgnr);
        break;
    case readphasecorrection:
        m_fPhaseCorrection = answer.toDouble(&ok);
        emit cmdDone(msgnr);
        break;
    }
}


void cRangeMeasChannel::setRangeListEntity()
{
    QStringList sl;
    QList<QString> keys;

    keys = m_RangeInfoHash.keys();
    for (int i = 0; i < keys.count(); i++)
        sl.append(keys.at(i));

    m_pChannelRangeListEntity->setValue(QVariant(sl));
}


void cRangeMeasChannel::setChannelEntity()
{
    m_pChannelEntity->setValue(QVariant(m_sAlias));
}


void cRangeMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceTypes()] = readresourcetypes;
}


void cRangeMeasChannel::readResource()
{
    m_MsgNrCmdList[m_pRMInterface->getResources("SENSE")] = readresource;
}


void cRangeMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_pRMInterface->getResourceInfo("SENSE", m_sName)] = readresourceinfo;
}


void cRangeMeasChannel::claimResource()
{
    m_MsgNrCmdList[m_pRMInterface->setResource("SENSE", m_sName, 1)] = claimresource;
}


void cRangeMeasChannel::pcbConnection()
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();
    m_pPCBInterface->setClient(m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_nPort));
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, QVariant)), this, SLOT(catchInterfaceAnswer(quint32,QVariant)));
    emit activationContinue();
}


void cRangeMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pPCBInterface->getDSPChannel(m_sName)] = readdspchannel;
}


void cRangeMeasChannel::readChnAlias()
{
   m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName)] = readchnalias;
}


void cRangeMeasChannel::readRangelist()
{
    m_MsgNrCmdList[m_pPCBInterface->getRangeList(m_sName)] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}


void cRangeMeasChannel::readRangeProperties1()
{
    if (m_RangeQueryIt <= m_RangeNameList.count()) // another range ?
        m_rangeQueryMachine.start(); // yes, fill it with information
    else
        emit activationContinue();
}


void cRangeMeasChannel::readRangeProperties2()
{
    m_RangeQueryIt++;
    emit activationContinue();
}


void cRangeMeasChannel::activationDone()
{
    QHash<QString, cRangeInfo>::iterator it = m_RangeInfoHash.begin();
    while (it != m_RangeInfoHash.end()) // we delete all unused ranges
    {
        ri = it.value();
        if (!ri.avail || ((ri.type & 1) == 0)) // in case range is not avail or virtual
            it = m_RangeInfoHash.erase(it);
        else
            ++it;
    }

    setChannelEntity(); // we set our real name now
    setRangeListEntity(); // and the list of possible ranges

    emit activated();
}


void cRangeMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cRangeMeasChannel::readType()
{
    m_MsgNrCmdList[m_pPCBInterface->getType(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}


void cRangeMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pPCBInterface->getUrvalue(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readurvalue;
}


void cRangeMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pPCBInterface->getRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readrejection;
}


void cRangeMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pPCBInterface->getOVRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readovrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pPCBInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cRangeMeasChannel::rangeQueryDone()
{
    ri.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHash[ri.alias] = ri; // for each range we append cRangeinfo per alias
}



