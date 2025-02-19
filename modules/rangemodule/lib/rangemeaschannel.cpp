#include "rangemeaschannelconvenience.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <QRegExp>
#include <math.h>

namespace RANGEMODULE
{

cRangeMeasChannel::cRangeMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                                     NetworkConnectionInfo pcbsocket,
                                     VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                     QString moduleName) :
    cBaseMeasChannel(pcbsocket,
                       tcpNetworkFactory,
                       channelObserver,
                       QString("%1/RangeMeasChannel/%2").arg(moduleName, channelObserver->getMName())),
    m_preScaling(1)
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();

    // setting up statemachine for "activating" meas channel
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_readRangeAndProperties.addTransition(&m_rangeQueryMachine, &QStateMachine::finished, &m_resetStatusState);
    m_resetStatusState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_setNotifierRangeCat);
    m_setNotifierRangeCat.addTransition(this, &cRangeMeasChannel::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_readRangeAndProperties);
    m_activationMachine.addState(&m_resetStatusState);
    m_activationMachine.addState(&m_setNotifierRangeCat);
    m_activationMachine.addState(&m_activationDoneState);

    connect(&m_pcbConnectionState, &QState::entered, this, &cRangeMeasChannel::pcbConnection);
    connect(&m_readRangeAndProperties, &QState::entered, this, &cRangeMeasChannel::readRangeAndProperties);
    connect(&m_resetStatusState, &QState::entered, this, &cRangeMeasChannel::resetStatusSlot);
    connect(&m_setNotifierRangeCat, &QState::entered, this, &cRangeMeasChannel::setNotifierRangeCat);
    connect(&m_activationDoneState, &QState::entered, this, &cRangeMeasChannel::activationDone);

    // setting up statemachine for "deactivating" meas channel
    m_deactivationInitState.addTransition(this, &cRangeMeasChannel::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_deactivationInitState);
    m_deactivationMachine.addState(&m_deactivationDoneState);

    connect(&m_deactivationInitState, &QState::entered, this, &cRangeMeasChannel::deactivationResetNotifiers);
    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeMeasChannel::deactivationDone);

    m_activationMachine.setInitialState(&m_pcbConnectionState);
    m_deactivationMachine.setInitialState(&m_deactivationInitState);

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


quint32 cRangeMeasChannel::setRange(const QString &range)
{
    m_sNewRange = range; // alias !!!!
    m_sActRange = range;

    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->setRange(getMName(), m_RangeInfoHash[range].name); // we set range per name not alias
        m_MsgNrCmdList[msgnr] = setmeaschannelrange;
        return msgnr;
    }
    else {
        return 1;
    }
}

QString cRangeMeasChannel::getRange() const
{
    return m_sActRange;
}

quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->getGainCorrection(getMName(), m_RangeInfoHash[m_sActRange].name, amplitude);
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
        quint32 msgnr = m_pcbInterface->getOffsetCorrection(getMName(), m_RangeInfoHash[m_sActRange].name, amplitude);
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
        quint32 msgnr = m_pcbInterface->getStatus(getMName());
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
        quint32 msgnr = m_pcbInterface->resetStatus(getMName());
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
        quint32 msgnr = m_pcbInterface->getPhaseCorrection(getMName(), m_RangeInfoHash[m_sActRange].name, frequency);
        m_MsgNrCmdList[msgnr] = readphasecorrection;
        return msgnr;
    }
    else {
        return 1;
    }
}


bool cRangeMeasChannel::isPossibleRange(const QString &range)
{
    return m_RangeInfoHash.contains(range);
}


bool cRangeMeasChannel::isRMSOverload(double ampl)
{
    const cRangeInfoWithConstantValues &ri = m_RangeInfoHash[m_sActRange];
    double ovrRejectionFactor = ri.ovrejection / ri.rejection;
    return ((ri.urvalue * ovrRejectionFactor) < ampl);
}


bool cRangeMeasChannel::isADCOverload(double ampl)
{
    const cRangeInfoWithConstantValues &ri = m_RangeInfoHash[m_sActRange];
    return (fabs(ri.adcrejection - ampl) < 8.0);
}


QString cRangeMeasChannel::getOptRange(double rms, const QString &rngAlias)
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

    QList<cRangeInfoWithConstantValues> riList = m_RangeInfoHash.values();
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
        const cRangeInfoWithConstantValues& ri = riList.at(i);
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


QString cRangeMeasChannel::getMaxRange() const
{
    QList<cRangeInfoWithConstantValues> riList = m_RangeInfoHash.values();
    double newAmpl = -1.0;
    int p=-1;

    for (int i = 0; i < riList.count(); i++) {
        double newUrvalue = riList.at(i).urvalue;
        if (newUrvalue > newAmpl) {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    return riList.at(p).alias;
}


QString cRangeMeasChannel::getMaxRange(const QString &rngAlias)
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

    QList<cRangeInfoWithConstantValues> riList = m_RangeInfoHash.values();
    double newAmpl = -1.0;
    int i, p = -1;

    for (i = 0; i < riList.count(); i++) {
        const cRangeInfoWithConstantValues& ri = riList.at(i);
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


double cRangeMeasChannel::getUrValue(const QString &range) const
{
    return m_RangeInfoHash[range].urvalue;
}

double cRangeMeasChannel::getRejection(const QString &range) const
{
    return m_RangeInfoHash[range].rejection;
}

double cRangeMeasChannel::getOVRRejection(const QString &range) const
{
    return m_RangeInfoHash[range].ovrejection;
}


void cRangeMeasChannel::generateVeinInterface()
{
}

RangeMeasChannelStateData *cRangeMeasChannel::getStateData()
{
    return &m_stateData;
}

void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

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
        int errcount = 0;
        switch (cmd)
        {
        case unregisterNotifiers:
            if (reply == ack)
                emit deactivationContinue();
            else
                notifyError(unregisterpcbnotifierErrMsg);
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
                m_CurrRangeInfo.alias = answer.toString();
                emit activationContinue();
            }
            else
                notifyError(readrangealiasErrMsg);
            break;

        case readtype:
            if (reply == ack) {
                m_CurrRangeInfo.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangetypeErrMsg);
            break;

        case readurvalue:
            if (reply == ack) {
                m_CurrRangeInfo.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeurvalueErrMsg);
            break;

        case readrejection:
            if (reply == ack) {
                m_CurrRangeInfo.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangerejectionErrMsg);
            break;

        case readovrejection:
            if (reply == ack) {
                m_CurrRangeInfo.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeovrejectionErrMsg);
            break;

        case readadcrejection:
            if (reply == ack) {
                m_CurrRangeInfo.adcrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeadcrejectionErrMsg);
            break;

        case readisavail:
            if (reply == ack) {
                m_CurrRangeInfo.avail = answer.toBool();
                emit activationContinue();
            }
            else
                notifyError(readrangeavailErrMsg);
            break;

        case setmeaschannelrange:
            if (reply == ack)
                m_sActRange = m_sNewRange;
            else {
                errcount = m_ActionErrorcountHash.take(setmeaschannelrange);
                errcount++;
                m_ActionErrorcountHash[setmeaschannelrange] = errcount;
                qWarning("Set range %s on %s failed with %i!",
                         qPrintable(m_sNewRange),
                         qPrintable(getAlias()),
                         reply);
                if (errcount > 1)
                    notifyError(setRangeErrMsg);
            } // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case readgaincorrection:
            if (reply == ack)
                m_fGainCorrection = answer.toDouble(&ok);
            else {
                errcount = m_ActionErrorcountHash.take(readgaincorrection);
                errcount++;
                m_ActionErrorcountHash[readgaincorrection] = errcount;
                if (errcount > 1)
                    notifyError(readGainCorrErrMsg);
            }
            emit cmdDone(msgnr);
            break;

        case readoffsetcorrection:
            if (reply == ack)
                m_fOffsetCorrection = answer.toDouble(&ok);
            else {
                errcount = m_ActionErrorcountHash.take(readoffsetcorrection);
                errcount++;
                m_ActionErrorcountHash[readoffsetcorrection] = errcount;
                if (errcount > 1)
                    notifyError(readOffsetCorrErrMsg);
            }
            emit cmdDone(msgnr);
            break;

        case readphasecorrection:
            if (reply == ack)
                m_fPhaseCorrection = answer.toDouble(&ok);
            else {
                errcount = m_ActionErrorcountHash.take(readphasecorrection);
                errcount++;
                m_ActionErrorcountHash[readphasecorrection] = errcount;
                if (errcount > 1)
                    notifyError(readPhaseCorrErrMsg);
            }
            emit cmdDone(msgnr);
            break;

        case readmeaschannelstatus:
            if (reply == ack)
                m_nStatus = answer.toInt(&ok);
            else
                notifyError(readChannelStatusErrMsg);
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus:
            if (reply == ack) { }
            else
                notifyError(resetChannelStatusErrMsg);
            emit cmdDone(msgnr);
            break;

        case resetmeaschannelstatus2:
            if (reply == ack)
                emit activationContinue();
            else
                notifyError(resetChannelStatusErrMsg);
            break;

        case registerNotifier:
            if (reply == ack)
                emit activationContinue();
            else
                notifyError(registerpcbnotifierErrMsg);
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
    QList<cRangeInfoWithConstantValues> riList = m_RangeInfoHash.values();
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

void cRangeMeasChannel::pcbConnection()
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(m_pcbNetworkInfo.m_sIP,
                                                                 m_pcbNetworkInfo.m_nPort,
                                                                 m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbClient.get(), &Zera::ProxyClient::connected, &m_readRangeAndProperties);

    m_pcbInterface->setClientSmart(m_pcbClient);
    connect(m_pcbInterface.get(), &Zera::cPCBInterface::serverAnswer, this, &cRangeMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
}


void cRangeMeasChannel::readRangeAndProperties()
{
    m_rangeQueryMachine.start(); // read all ranges and their properties
}


void cRangeMeasChannel::resetStatusSlot()
{
    m_MsgNrCmdList[m_pcbInterface->resetStatus(getMName())] = resetmeaschannelstatus2;
}


void cRangeMeasChannel::setNotifierRangeCat()
{
    m_MsgNrCmdList[m_pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(getMName()), 1)] = registerNotifier;
}


void cRangeMeasChannel::activationDone()
{
    m_bActive = true;
    emit activated();
}


void cRangeMeasChannel::deactivationResetNotifiers()
{
    m_bActive = false;
    m_MsgNrCmdList[m_pcbInterface->unregisterNotifiers()] = unregisterNotifiers;
}


void cRangeMeasChannel::deactivationDone()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pcbClient.get());
    // and disconnect for our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}


void cRangeMeasChannel::readRangelist()
{
    m_RangeInfoHashWorking.clear();
    m_MsgNrCmdList[m_pcbInterface->getRangeList(getMName())] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}



void cRangeMeasChannel::readRngAlias()
{
    m_MsgNrCmdList[m_pcbInterface->getAlias(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readrngalias;
}


void cRangeMeasChannel::readType()
{
    m_MsgNrCmdList[m_pcbInterface->getType(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readtype;
}


void cRangeMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pcbInterface->getUrvalue(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readurvalue;
}


void cRangeMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pcbInterface->getRejection(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readrejection;
}


void cRangeMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pcbInterface->getOVRejection(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readovrejection;
}


void cRangeMeasChannel::readADCRejection()
{
    m_MsgNrCmdList[m_pcbInterface->getADCRejection(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readadcrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pcbInterface->isAvail(getMName(), m_RangeNameList.at(m_RangeQueryIt))] = readisavail;
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
        QHash<QString, cRangeInfoWithConstantValues>::iterator it = m_RangeInfoHashWorking.begin();
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
