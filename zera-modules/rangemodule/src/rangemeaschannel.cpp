#include "rangemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <QRegExp>
#include <math.h>

namespace RANGEMODULE
{

cRangeMeasChannel::cRangeMeasChannel(cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr, bool demo)
    :cBaseMeasChannel(rmsocket, pcbsocket, name, chnnr), m_bDemo(demo),m_preScaling(1)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" meas channel
    // m_rmConnectState.addTransition is done in rmConnect
    m_IdentifyState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readResourceTypesState);
    m_readResourceTypesState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readResourceState);
    m_readResourceState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readResourceInfoState);
    m_readResourceInfoState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_claimResourceState);
    m_claimResourceState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_pcbConnectionState);
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readDspChannelState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readChnAliasState);
    m_readChnAliasState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readSampleRateState);
    m_readSampleRateState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readUnitState);
    m_readUnitState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readRangeAndProperties);
    m_readRangeAndProperties.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_resetStatusState);
    m_resetStatusState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_setNotifierRangeCat);
    m_setNotifierRangeCat.addTransition(this, &cRangeMeasChannel::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_rmConnectState);
    m_activationMachine.addState(&m_IdentifyState);
    m_activationMachine.addState(&m_readResourceTypesState);
    m_activationMachine.addState(&m_readResourceState);
    m_activationMachine.addState(&m_readResourceInfoState);
    m_activationMachine.addState(&m_claimResourceState);
    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readDspChannelState);
    m_activationMachine.addState(&m_readChnAliasState);
    m_activationMachine.addState(&m_readSampleRateState);
    m_activationMachine.addState(&m_readUnitState);
    m_activationMachine.addState(&m_readRangeAndProperties);
    m_activationMachine.addState(&m_resetStatusState);
    m_activationMachine.addState(&m_setNotifierRangeCat);
    m_activationMachine.addState(&m_activationDoneState);

    m_activationMachine.setInitialState(&m_rmConnectState);

    connect(&m_rmConnectState, &QState::entered, this, &cRangeMeasChannel::rmConnect);
    connect(&m_IdentifyState, &QState::entered, this, &cRangeMeasChannel::sendRMIdent);
    connect(&m_readResourceTypesState, &QState::entered, this, &cRangeMeasChannel::readResourceTypes);
    connect(&m_readResourceState, &QState::entered, this, &cRangeMeasChannel::readResource);
    connect(&m_readResourceInfoState, &QState::entered, this, &cRangeMeasChannel::readResourceInfo);
    connect(&m_claimResourceState, &QState::entered, this, &cRangeMeasChannel::claimResource);
    connect(&m_pcbConnectionState, &QState::entered, this, &cRangeMeasChannel::pcbConnection);
    connect(&m_readDspChannelState, &QState::entered, this, &cRangeMeasChannel::readDspChannel);
    connect(&m_readChnAliasState, &QState::entered, this, &cRangeMeasChannel::readChnAlias);
    connect(&m_readSampleRateState, &QState::entered, this, &cRangeMeasChannel::readSampleRate);
    connect(&m_readUnitState, &QState::entered, this, &cRangeMeasChannel::readUnit);
    connect(&m_readRangeAndProperties, &QState::entered, this, &cRangeMeasChannel::readRangeAndProperties);
    connect(&m_resetStatusState, &QState::entered, this, &cRangeMeasChannel::resetStatusSlot);
    connect(&m_setNotifierRangeCat, &QState::entered, this, &cRangeMeasChannel::setNotifierRangeCat);
    connect(&m_activationDoneState, &QState::entered, this, &cRangeMeasChannel::activationDone);

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, &cRangeMeasChannel::deactivationContinue, &m_deactivationResetNotifiersState);
    m_deactivationResetNotifiersState.addTransition(this, &cRangeMeasChannel::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationResetNotifiersState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    if(!m_bDemo) {
        m_deactivationMachine.setInitialState(&m_deactivationInitState);
    } else {
        m_deactivationMachine.setInitialState(&m_deactivationDoneState);
    }
    connect(&m_deactivationInitState, &QState::entered, this, &cRangeMeasChannel::deactivationInit);
    connect(&m_deactivationResetNotifiersState, &QState::entered, this, &cRangeMeasChannel::deactivationResetNotifiers);
    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeMeasChannel::deactivationDone);

    // setting up statemachine for querying the meas channels ranges and their properties
    m_readRangelistState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readRngAliasState);
    m_readRngAliasState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readTypeState);
    m_readTypeState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readUrvalueState);
    m_readUrvalueState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readRejectionState);
    m_readRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readOVRejectionState);
    m_readOVRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readADCRejectionState);
    m_readADCRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readisAvailState);
    m_readisAvailState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_rangeQueryLoopState);
    m_rangeQueryLoopState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_rangeQueryDoneState);
    m_rangeQueryLoopState.addTransition(this, &cRangeMeasChannel::activationLoop, &m_readRngAliasState);

    m_rangeQueryMachine.addState(&m_readRangelistState);
    m_rangeQueryMachine.addState(&m_readRngAliasState);
    m_rangeQueryMachine.addState(&m_readTypeState);
    m_rangeQueryMachine.addState(&m_readUrvalueState);
    m_rangeQueryMachine.addState(&m_readRejectionState);
    m_rangeQueryMachine.addState(&m_readOVRejectionState);
    m_rangeQueryMachine.addState(&m_readADCRejectionState);
    m_rangeQueryMachine.addState(&m_readisAvailState);
    m_rangeQueryMachine.addState(&m_rangeQueryLoopState);
    m_rangeQueryMachine.addState(&m_rangeQueryDoneState);

    m_rangeQueryMachine.setInitialState(&m_readRangelistState);

    connect(&m_readRangelistState, &QState::entered, this, &cRangeMeasChannel::readRangelist);
    connect(&m_readRngAliasState, &QState::entered, this, &cRangeMeasChannel::readRngAlias);
    connect(&m_readTypeState, &QState::entered, this, &cRangeMeasChannel::readType);
    connect(&m_readUrvalueState, &QState::entered, this, &cRangeMeasChannel::readUrvalue);
    connect(&m_readRejectionState, &QState::entered, this, &cRangeMeasChannel::readRejection);
    connect(&m_readOVRejectionState, &QState::entered, this, &cRangeMeasChannel::readOVRejection);
    connect(&m_readADCRejectionState, &QState::entered, this, &cRangeMeasChannel::readADCRejection);
    connect(&m_readisAvailState, &QState::entered, this, &cRangeMeasChannel::readisAvail);
    connect(&m_rangeQueryLoopState, &QState::entered, this, &cRangeMeasChannel::rangeQueryLoop);

    connect(&m_rangeQueryMachine, &QStateMachine::finished, this, &cRangeMeasChannel::newRangeList);
    setActionErrorcount(1);
}


cRangeMeasChannel::~cRangeMeasChannel()
{
    delete m_pPCBInterface;
}


quint32 cRangeMeasChannel::setRange(QString range)
{
    m_sNewRange = range; // alias !!!!
    m_sActRange = range;

    if(m_bDemo) {
        return 0;
    }
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->setRange(m_sName, m_RangeInfoHash[range].name); // we set range per name not alias
        m_MsgNrCmdList[msgnr] = setmeaschannelrange;
        return msgnr;
    }
    else {
        return 1;
    }
}


quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->getGainCorrection(m_sName, m_RangeInfoHash[m_sActRange].name, amplitude);
        m_MsgNrCmdList[msgnr] = readgaincorrection;
        return msgnr;
    }
    else {
        return 1;
    }
}


quint32 cRangeMeasChannel::readOffsetCorrection(double amplitude)
{
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->getOffsetCorrection(m_sName, m_RangeInfoHash[m_sActRange].name, amplitude);
        m_MsgNrCmdList[msgnr] = readoffsetcorrection;
        return msgnr;
    }
    else {
        return 1;
    }
}

// This caused a bit of confusion so write it down:
// com5003d / mt310d read (global) critical status for each channnel and
// check only that bit belonging to the channnel
quint32 cRangeMeasChannel::readStatus()
{
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->getStatus(m_sName);
        m_MsgNrCmdList[msgnr] = readmeaschannelstatus;
        return msgnr;
    }
    else {
        return 1;
    }
}


quint32 cRangeMeasChannel::resetStatus()
{
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->resetStatus(m_sName);
        m_MsgNrCmdList[msgnr] = resetmeaschannelstatus;
        return msgnr;
    }
    else {
        return 1;
    }
}


quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    if (m_bActive) {
        quint32 msgnr = m_pPCBInterface->getPhaseCorrection(m_sName, m_RangeInfoHash[m_sActRange].name, frequency);
        m_MsgNrCmdList[msgnr] = readphasecorrection;
        return msgnr;
    }
    else {
        return 1;
    }
}


bool cRangeMeasChannel::isPossibleRange(QString range)
{
    return m_RangeInfoHash.contains(range);
}


bool cRangeMeasChannel::isRMSOverload(double ampl)
{
    cRangeInfo& ri = m_RangeInfoHash[m_sActRange];
    double ovrRejectionFactor = ri.ovrejection / ri.rejection;
    return ((ri.urvalue * ovrRejectionFactor) < ampl);
}


bool cRangeMeasChannel::isADCOverload(double ampl)
{
    cRangeInfo& ri = m_RangeInfoHash[m_sActRange];
    return (fabs(ri.adcrejection - ampl) < 8.0);
}


QString cRangeMeasChannel::getOptRange(double rms, QString rngAlias)
{
    qint32 actRngType = -1;
    if (m_RangeInfoHash.contains(rngAlias)) {
        // if we know this rngalias we take that's type for searching max range
        actRngType = m_RangeInfoHash[rngAlias].type; // the type of actual range
    }
    else {
        // if we already set a range we could take the actual range for searching
        if (m_RangeInfoHash.contains(m_sActRange)) {
            actRngType = m_RangeInfoHash[m_sActRange].type;
        }
    }

    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = 1e32;
    int i, p = -1;

    // Decision areas are:
    //-------- Rejection
    // ------- keepRangeLimit
    //     if value is inside hysteresis area: keep range
    // ------- enterRangeLimit
    //     if value is in this area we can enter range
    //
    // ------- zero

    // keep area < 1: avoid overload because overload passes maximum range and
    // that wastes our time unnecessesarily
    const double keepRangeLimit = 0.99;
    const double enterRangeLimit = 0.95;

    for (i = 0; i < riList.count(); i++) {
        const cRangeInfo& ri = riList.at(i);
        double newUrvalue = ri.urvalue;
        double ovrRejectionFactor = ri.ovrejection / ri.rejection; // typically 1.25
        // actual range?
        if(rngAlias == ri.alias) {
            // are we in hysteresis area?
            if(rms > newUrvalue * ovrRejectionFactor * enterRangeLimit &&
                    rms < newUrvalue * ovrRejectionFactor * keepRangeLimit) {
                // let's keep actual range
                p=i;
                break;
            }
        }
        // (re-)enter range
        if ((rms <= newUrvalue * ovrRejectionFactor * enterRangeLimit) && (newUrvalue < newAmpl) && (ri.type == actRngType)) {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    if (p > -1) {
        return riList.at(p).alias;
    }
    else {
        return getMaxRange(rngAlias); // we return maximum range in case of overload condition
    }
}


QString cRangeMeasChannel::getMaxRange()
{
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = -1.0;
    double newUrvalue;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++) {
        newUrvalue = riList.at(i).urvalue;
        if (newUrvalue > newAmpl) {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    return riList.at(p).alias;
}


QString cRangeMeasChannel::getMaxRange(QString rngAlias)
{
    qint32 actRngType = -1;
    if (m_RangeInfoHash.contains(rngAlias)) {
        // if we know this rngalias we take that's type for searching max range
        actRngType = m_RangeInfoHash[rngAlias].type; // the type of actual range
    }
    else {
        // if we already set a range we could take the actual range for searching
        if (m_RangeInfoHash.contains(m_sActRange)) {
            actRngType = m_RangeInfoHash[m_sActRange].type;
        }
    }

    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    double newAmpl = -1.0;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++) {
        const cRangeInfo& ri = riList.at(i);
        double newUrvalue = ri.urvalue;
        if (actRngType >=0) {
            if ((newUrvalue > newAmpl) && (ri.type == actRngType)) {
                newAmpl = newUrvalue;
                p=i;
            }
        }
        else {
            if ((newUrvalue > newAmpl)) {
                newAmpl = newUrvalue;
                p=i;
            }
        }
    }

    return riList.at(p).alias;
}


QString cRangeMeasChannel::getRangeListAlias()
{
    return m_sRangeListAlias;
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


double cRangeMeasChannel::getMaxRangeUrvalueMax()
{
   QString s = getMaxRange();
   return (getUrValue(s) * getOVRRejection(s) / getRejection(s));
}


double cRangeMeasChannel::getRangeUrvalueMax()
{
    return (getUrValue() * getOVRRejection() / getRejection());
}


void cRangeMeasChannel::generateInterface()
{
}


void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;
    int errcount;

    if (msgnr == 0) {// 0 was reserved for async. messages
        QString sintnr;
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service) {
        case 1:
            // we got a sense:chn:range:cat notifier
            // so we have to read the new range list and properties
            if (!m_rangeQueryMachine.isRunning()) {
                setActionErrorcount(0);
                m_rangeQueryMachine.start();
            }
            break;
        }
    }

    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd)
        {
        case sendmeaschannelrmident:
            if (reply == ack) { // we only continue if resource manager acknowledges
                emit activationContinue();
            }
            else {
                emit errMsg(tr(rmidentErrMSG));
                emit activationError();
            }
            break;

        case readresourcetypes:
            if ((reply == ack) && (answer.toString().contains("SENSE"))) {
                emit activationContinue();
            }
            else {
                emit errMsg((tr(resourcetypeErrMsg)));
                emit activationError();
            }
            break;

        case readresource:
            if ((reply == ack) && (answer.toString().contains(m_sName))) {
                emit activationContinue();
            }
            else {
                emit errMsg((tr(resourceErrMsg)));
                emit activationError();
            }
            break;

        case readresourceinfo:
        {
            bool ok1, ok2, ok3;
            int max, free;
            QStringList sl;


            sl = answer.toString().split(';');
            if ((reply ==ack) && (sl.length() >= 4)) {
                max = sl.at(0).toInt(&ok1); // fixed position
                free = sl.at(1).toInt(&ok2);
                m_sDescription = sl.at(2);
                m_nPort = sl.at(3).toInt(&ok3);

                if (ok1 && ok2 && ok3 && ((max == free) == 1)) {
                    emit activationContinue();
                }

                else {
                    emit errMsg((tr(resourceInfoErrMsg)));
                    emit activationError();
                }
            }

            else {
                emit errMsg((tr(resourceInfoErrMsg)));
                emit activationError();
            }
            break;
        }
        case claimresource:
            if (reply == ack) {
                emit activationContinue();
            }
            else {
                emit errMsg((tr(claimresourceErrMsg)));
                emit activationError();
            }
            break;

        case freeresource:
            if (reply == ack || reply == nack) { // we accept nack here also
                emit deactivationContinue(); // maybe that resource was deleted by server and then it is no more set
            }
            else {
                emit errMsg((tr(freeresourceErrMsg)));
                emit deactivationError();
            }
            break;

        case unregisterNotifiers:
            if (reply == ack) {
                emit deactivationContinue();
            }
            else {
                emit errMsg((tr(unregisterpcbnotifierErrMsg)));
                emit deactivationError();
            }
            break;

        case readdspchannel:
            if (reply == ack) {
                m_nDspChannel = answer.toInt(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readdspchannelErrMsg)));
                emit activationError();
            }
            break;

        case readchnalias:
            if (reply == ack) {
                m_sAlias = answer.toString();
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readaliasErrMsg)));
                emit activationError();
            }
            break;

        case readsamplerate:
            if (reply == ack) {
                m_nSampleRate = answer.toInt(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readsamplerateErrMsg)));
                emit activationError();
            }
            break;

        case readunit:
            if (reply == ack) {
                m_sUnit = answer.toString();
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readunitErrMsg)));
                emit activationError();
            }
            break;

        case readrangelist:
            if (reply == ack) {
                m_RangeNameList = answer.toStringList();
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangelistErrMsg)));
                emit activationError();
            }
            break;

        case readrngalias:
            if (reply == ack) {
                m_CurrRangeInfo.alias = answer.toString();
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangealiasErrMsg)));
                emit activationError();
            }
            break;

        case readtype:
            if (reply == ack) {
                m_CurrRangeInfo.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangetypeErrMsg)));
                emit activationError();
            }
            break;

        case readurvalue:
            if (reply == ack) {
                m_CurrRangeInfo.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangeurvalueErrMsg)));
                emit activationError();
            }
            break;

        case readrejection:
            if (reply == ack) {
                m_CurrRangeInfo.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangerejectionErrMsg)));
                emit activationError();
            }
            break;

        case readovrejection:
            if (reply == ack) {
                m_CurrRangeInfo.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangeovrejectionErrMsg)));
                emit activationError();
            }
            break;

        case readadcrejection:
            if (reply == ack) {
                m_CurrRangeInfo.adcrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangeadcrejectionErrMsg)));
                emit activationError();
            }
            break;

        case readisavail:
            if (reply == ack) {
                m_CurrRangeInfo.avail = answer.toBool();
                emit activationContinue();
            }
            else {
                emit errMsg((tr(readrangeavailErrMsg)));
                emit activationError();
            }
            break;

        case setmeaschannelrange:
            if (reply == ack) {
                m_sActRange = m_sNewRange;
            }
            else {
                errcount = m_ActionErrorcountHash.take(setmeaschannelrange);
                errcount++;
                m_ActionErrorcountHash[setmeaschannelrange] = errcount;
                qWarning("Set range %s on %s failed with %i!",
                         qPrintable(m_sNewRange),
                         qPrintable(getAlias()),
                         reply);
                if (errcount > 1) {
                    emit errMsg((tr(setRangeErrMsg)));
                    emit executionError();
                }
            } // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case readgaincorrection:
            if (reply == ack) {
                m_fGainCorrection = answer.toDouble(&ok);
            }
            else {
                errcount = m_ActionErrorcountHash.take(readgaincorrection);
                errcount++;
                m_ActionErrorcountHash[readgaincorrection] = errcount;
                if (errcount > 1) {
                    emit errMsg((tr(readGainCorrErrMsg)));
                    emit executionError();
                }
            }
            emit cmdDone(msgnr);
            break;

        case readoffsetcorrection:
            if (reply == ack) {
                m_fOffsetCorrection = answer.toDouble(&ok);
            }
            else {
                errcount = m_ActionErrorcountHash.take(readoffsetcorrection);
                errcount++;
                m_ActionErrorcountHash[readoffsetcorrection] = errcount;
                if (errcount > 1) {
                    emit errMsg((tr(readOffsetCorrErrMsg)));
                    emit executionError();
                }
            }
            emit cmdDone(msgnr);
            break;

        case readphasecorrection:
            if (reply == ack) {
                m_fPhaseCorrection = answer.toDouble(&ok);
            }
            else {
                errcount = m_ActionErrorcountHash.take(readphasecorrection);
                errcount++;
                m_ActionErrorcountHash[readphasecorrection] = errcount;
                if (errcount > 1)
                {
                    emit errMsg((tr(readPhaseCorrErrMsg)));
                    emit executionError();
                }
            }
            emit cmdDone(msgnr);
            break;

        case readmeaschannelstatus:
            if (reply == ack) {
                m_nStatus = answer.toInt(&ok);
            }
            else {
                emit errMsg((tr(readChannelStatusErrMsg)));
                emit executionError();
            }
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus:
            if (reply == ack) {
            }
            else {
                emit errMsg((tr(resetChannelStatusErrMsg)));
                emit executionError();
            } // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus2:
            if (reply == ack) {
                emit activationContinue();
            }
            else {
                emit errMsg((tr(resetChannelStatusErrMsg)));
                emit activationError();
            } // perhaps some error output
            break;

        case registerNotifier:
            if (reply == ack) {
                emit activationContinue();
            }
            else {
                emit errMsg((tr(registerpcbnotifierErrMsg)));
                emit activationError();
            } // perhaps some error output
            break;
        }
    }
}

double cRangeMeasChannel::getPeakValueWithDc() const
{
    return m_peakValueWithDc;
}

void cRangeMeasChannel::setPeakValueWithDc(double peakValueWithDc)
{
    m_peakValueWithDc = peakValueWithDc;
}

double cRangeMeasChannel::getPeakValue() const
{
    return m_peakValue;
}

void cRangeMeasChannel::setPeakValue(double peakValue)
{
    m_peakValue = peakValue;
}

double cRangeMeasChannel::getSignalFrequency() const
{
    return m_signalFrequency;
}

void cRangeMeasChannel::setSignalFrequency(double signalFrequency)
{
    m_signalFrequency = signalFrequency;
}

double cRangeMeasChannel::getRmsValue() const
{
    return m_rmsValue;
}

void cRangeMeasChannel::setRmsValue(double actualValue)
{
    m_rmsValue = actualValue;
}

double cRangeMeasChannel::getPreScaling() const
{
    return m_preScaling;
}

void cRangeMeasChannel::setPreScaling(double preScaling)
{
    m_preScaling = preScaling;
}


void cRangeMeasChannel::setRangeListAlias()
{
    QString s;
    QList<cRangeInfo> riList = m_RangeInfoHash.values();
    int riLen;

    if ( (riLen = riList.count()) > 1) {// nothing to sort if we only have 1 range
        // first we sort the range alias according to range type
        for (int i = 0; i < riLen-1; i++) {
            for (int j = i; j < riLen; j++) {
                if (riList.at(i).type < riList.at(j).type) {
                    riList.swapItemsAt(i, j);
                }
            }
        }

        // second we sort the range alias according to upper range values but grouped with types
        for (int i = 0; i < riLen-1; i++) {
            for (int j = i; j < riLen; j++) {
                if ( (riList.at(i).urvalue < riList.at(j).urvalue) && (riList.at(i).type == riList.at(j).type) ) {
                    riList.swapItemsAt(i, j);
                }
            }
        }
    }

    s = riList.at(0).alias;

    if (riLen > 1) {
        for (int i = 1; i < riList.count(); i++) {
            s = s + ";" + riList.at(i).alias;
        }
    }

    m_sRangeListAlias = s;
}


void cRangeMeasChannel::setActionErrorcount(int Count)
{
    m_ActionErrorcountHash.clear();
    m_ActionErrorcountHash[readgaincorrection] = Count;
    m_ActionErrorcountHash[readphasecorrection] = Count;
    m_ActionErrorcountHash[readoffsetcorrection] = Count;
    m_ActionErrorcountHash[setmeaschannelrange] = Count;
}

void cRangeMeasChannel::setupDemoOperation()
{
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
    for(auto rangeVal : nominalRanges) {
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
    setRangeListAlias();
}


void cRangeMeasChannel::rmConnect()
{
    if(!m_bDemo) {
        // we instantiate a working resource manager interface first
        // so first we try to get a connection to resource manager over proxy
        m_rmClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pRMSocket->m_sIP, m_pRMSocket->m_nPort);
        m_rmConnectState.addTransition(m_rmClient.get(), &Zera::ProxyClient::connected, &m_IdentifyState);
        // and then we set connection resource manager interface's connection
        m_rmInterface.setClientSmart(m_rmClient); //
        // todo insert timer for timeout

        connect(&m_rmInterface, &Zera::Server::cRMInterface::serverAnswer, this, &cRangeMeasChannel::catchInterfaceAnswer);
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
    else {
        // add bits not done due to missing server responses
        setupDemoOperation();
        // make state machine finish
        m_rmConnectState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_activationDoneState);
        emit activationContinue();
    }
}


void cRangeMeasChannel::sendRMIdent()
{
   m_MsgNrCmdList[m_rmInterface.rmIdent(QString("MeasChannel%1").arg(m_nChannelNr))] = sendmeaschannelrmident;
}


void cRangeMeasChannel::readResourceTypes()
{
    m_MsgNrCmdList[m_rmInterface.getResourceTypes()] = readresourcetypes;
}


void cRangeMeasChannel::readResource()
{
    m_MsgNrCmdList[m_rmInterface.getResources("SENSE")] = readresource;
}


void cRangeMeasChannel::readResourceInfo()
{
    m_MsgNrCmdList[m_rmInterface.getResourceInfo("SENSE", m_sName)] = readresourceinfo;
}


void cRangeMeasChannel::claimResource()
{
    m_MsgNrCmdList[m_rmInterface.setResource("SENSE", m_sName, 1)] = claimresource;
}


void cRangeMeasChannel::pcbConnection()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(m_pPCBServerSocket->m_sIP, m_nPort);
    m_pcbConnectionState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_readDspChannelState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cRangeMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cRangeMeasChannel::readDspChannel()
{
   m_MsgNrCmdList[m_pPCBInterface->getDSPChannel(m_sName)] = readdspchannel;
}


void cRangeMeasChannel::readChnAlias()
{
    m_MsgNrCmdList[m_pPCBInterface->getAlias(m_sName)] = readchnalias;
}


void cRangeMeasChannel::readSampleRate()
{
    m_MsgNrCmdList[m_pPCBInterface->getSampleRate()] = readsamplerate;
}


void cRangeMeasChannel::readUnit()
{
    m_MsgNrCmdList[m_pPCBInterface->getUnit(m_sName)] = readunit;
}


void cRangeMeasChannel::readRangeAndProperties()
{
    m_rangeQueryMachine.start(); // read all ranges and their properties
}


void cRangeMeasChannel::resetStatusSlot()
{
    m_MsgNrCmdList[m_pPCBInterface->resetStatus(m_sName)] = resetmeaschannelstatus2;
}


void cRangeMeasChannel::setNotifierRangeCat()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(m_sName), 1)] = registerNotifier;
}


void cRangeMeasChannel::activationDone()
{
    m_bActive = true;
    emit activated();
}


void cRangeMeasChannel::deactivationInit()
{
    // deactivation means we have to free our resources
    m_bActive = false;
    m_MsgNrCmdList[m_rmInterface.freeResource("SENSE", m_sName)] = freeresource;
}


void cRangeMeasChannel::deactivationResetNotifiers()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisterNotifiers;
}


void cRangeMeasChannel::deactivationDone()
{
    if(!m_bDemo) {
        Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
        // and disconnect for our servers afterwards
        disconnect(&m_rmInterface, 0, this, 0);
        disconnect(m_pPCBInterface, 0, this, 0);
    }
    emit deactivated();
}


void cRangeMeasChannel::readRangelist()
{
    m_RangeInfoHashWorking.clear();
    m_MsgNrCmdList[m_pPCBInterface->getRangeList(m_sName)] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
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


void cRangeMeasChannel::readADCRejection()
{
    m_MsgNrCmdList[m_pPCBInterface->getADCRejection(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readadcrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pPCBInterface->isAvail(m_sName, m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
}


void cRangeMeasChannel::rangeQueryLoop()
{
    m_CurrRangeInfo.name = m_RangeNameList.at(m_RangeQueryIt);
    m_RangeInfoHashWorking[m_CurrRangeInfo.alias] = m_CurrRangeInfo; // for each range we append cRangeinfo per alias

    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_RangeNameList.count()) { // another range ?
        emit activationLoop();
    }
    else {
        QHash<QString, cRangeInfo>::iterator it = m_RangeInfoHashWorking.begin();
        while (it != m_RangeInfoHashWorking.end()) { // we delete all unused ranges
            m_CurrRangeInfo= it.value();
            if (m_CurrRangeInfo.avail) {
                ++it;
            }
            else {
                it = m_RangeInfoHashWorking.erase(it); // in case range is not avail
            }
        }

        m_RangeInfoHash = m_RangeInfoHashWorking;

        setRangeListAlias(); // and the list of possible ranges alias
        emit activationContinue();
    }
}


}
