#include "rangemeaschannel.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>
#include <QRegExp>
#include <math.h>

namespace RANGEMODULE
{

cRangeMeasChannel::cRangeMeasChannel(const ChannelRangeObserver::ChannelPtr &channelObserver,
                                     const NetworkConnectionInfo &pcbsocket,
                                     const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                     const QString &moduleName) :
    cBaseMeasChannel(pcbsocket,
                     tcpNetworkFactory,
                     channelObserver,
                     QString("%1/RangeMeasChannel/%2").arg(moduleName, channelObserver->getMName()))
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
    m_readRangelistState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readTypeState);
    m_readTypeState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readUrvalueState);
    m_readUrvalueState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readRejectionState);
    m_readRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readOVRejectionState);
    m_readOVRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readADCRejectionState);
    m_readADCRejectionState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_readisAvailState);
    m_readisAvailState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_rangeQueryLoopState);
    m_rangeQueryLoopState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_rangeQueryDoneState);
    m_rangeQueryLoopState.addTransition(this, &cRangeMeasChannel::activationLoop, &m_readTypeState);

    m_rangeQueryMachine.addState(&m_readRangelistState);
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
    connect(&m_readTypeState, &QState::entered, this, &cRangeMeasChannel::readType);
    connect(&m_readUrvalueState, &QState::entered, this, &cRangeMeasChannel::readUrvalue);
    connect(&m_readRejectionState, &QState::entered, this, &cRangeMeasChannel::readRejection);
    connect(&m_readOVRejectionState, &QState::entered, this, &cRangeMeasChannel::readOVRejection);
    connect(&m_readADCRejectionState, &QState::entered, this, &cRangeMeasChannel::readADCRejection);
    connect(&m_readisAvailState, &QState::entered, this, &cRangeMeasChannel::readisAvail);
    connect(&m_rangeQueryLoopState, &QState::entered, this, &cRangeMeasChannel::rangeQueryLoop);

    connect(&m_rangeQueryMachine, &QStateMachine::finished, this, &cRangeMeasChannel::newRangeList);
}

enum rangemeaschannelCmds
{
    readrangelist,

    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readadcrejection,
    readisavail,

    setmeaschannelrange,
    readgaincorrectioninternalandclamp,
    readoffsetcorrection,
    readphasecorrection,
    readchannelstatusbitmask,
    resetchannelstatusbitmask_emitcmddone,
    resetchannelstatusbitmask_instatemachine,
    registerNotifier,
    unregisterNotifiers
};

quint32 cRangeMeasChannel::setRange(const QString &rangeName)
{
    m_nextRangeName = rangeName;
    m_actRangeName = rangeName;

    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->setRange(getMName(), rangeName);
        m_MsgNrCmdList[msgnr] = setmeaschannelrange;
        return msgnr;
    }
    else {
        return 1;
    }
}

QString cRangeMeasChannel::getCurrRangeName() const
{
    return m_actRangeName;
}

quint32 cRangeMeasChannel::readGainCorrection(double amplitude)
{
    if (m_bActive) {
        ChannelCommonStorage *channelCommon = getChannelObserver()->getModuleCommonStorage();
        channelCommon->setLastGainAdjAmplitude(amplitude);
        quint32 msgnr = m_pcbInterface->getGainCorrection(getMName(), m_actRangeName, amplitude);
        m_MsgNrCmdList[msgnr] = readgaincorrectioninternalandclamp;
        return msgnr;
    }
    else {
        return 1;
    }
}


quint32 cRangeMeasChannel::readOffsetCorrection(double amplitude)
{
    if (m_bActive) {
        ChannelCommonStorage *channelCommon = getChannelObserver()->getModuleCommonStorage();
        channelCommon->setLastOffsetAdjAmplitude(amplitude);
        quint32 msgnr = m_pcbInterface->getOffsetCorrection(getMName(), m_actRangeName, amplitude);
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
        m_MsgNrCmdList[msgnr] = readchannelstatusbitmask;
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
        m_MsgNrCmdList[msgnr] = resetchannelstatusbitmask_emitcmddone;
        return msgnr;
    }
    else {
        return 1;
    }
}

quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->getPhaseCorrection(getMName(), m_actRangeName, frequency);
        m_MsgNrCmdList[msgnr] = readphasecorrection;
        return msgnr;
    }
    else {
        return 1;
    }
}

bool cRangeMeasChannel::isPossibleRange(const QString &rangeName)
{
    return m_rangeNameToChannelData.contains(rangeName);
}

bool cRangeMeasChannel::isRMSOverload(double ampl)
{
    const cRangeInfoWithConstantValues &ri = m_rangeNameToChannelData[m_actRangeName];
    double ovrRejectionFactor = ri.ovrejection / ri.rejection;
    return ((ri.urvalue * ovrRejectionFactor) < ampl);
}

bool cRangeMeasChannel::isPeakOverload(double peak)
{
    const cRangeInfoWithConstantValues &ri = m_rangeNameToChannelData[m_actRangeName];
    double ovrRejectionFactor = ri.ovrejection / ri.rejection;
    double overloadlimit = ri.urvalue * ovrRejectionFactor * M_SQRT2;
    return (overloadlimit < peak);
}

QString cRangeMeasChannel::getOptimalRange(double rms, double peak, const QString &rangeName)
{
    qint32 actRngType = -1;
    if (m_rangeNameToChannelData.contains(rangeName)) {
        // if we know this rngname we take that's type for searching max range
        actRngType = m_rangeNameToChannelData[rangeName].type; // the type of actual range
    }
    else {
        // if we already set a range we could take the actual range for searching
        if (m_rangeNameToChannelData.contains(m_actRangeName)) {
            actRngType = m_rangeNameToChannelData[m_actRangeName].type;
        }
    }
    QString newRangeRmsStr = findOptimalRange(rms, rangeName, actRngType, 1);
    double newRangeRmsUrValue = m_rangeNameToChannelData[newRangeRmsStr].urvalue;
    QString newRangePeakStr = findOptimalRange(peak, rangeName, actRngType, M_SQRT2);
    double newRangePeakUrValue = m_rangeNameToChannelData[newRangePeakStr].urvalue;

    return newRangePeakUrValue > newRangeRmsUrValue ? newRangePeakStr : newRangeRmsStr;
}

QString cRangeMeasChannel::findOptimalRange(double actualValue, const QString &actRangeName, qint32 actRngType, double extraRejectionFactor)
{
    QList<cRangeInfoWithConstantValues> riList = m_rangeNameToChannelData.values();
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

    for (i = 0; i < riList.count(); i++) {
        const cRangeInfoWithConstantValues& ri = riList.at(i);
        double newUrvalue = ri.urvalue;
        double ovrRejectionFactor = (ri.ovrejection / ri.rejection) * extraRejectionFactor; // typically 1.25
        // actual range?
        if(actRangeName == ri.name) {
            // are we in hysteresis area?
            if(actualValue > newUrvalue * ovrRejectionFactor * enterRangeLimit &&
                actualValue < newUrvalue * ovrRejectionFactor * keepRangeLimit) {
                // let's keep actual range
                p=i;
                break;
            }
        }
        // (re-)enter range
        if ((actualValue <= newUrvalue * ovrRejectionFactor * enterRangeLimit) && (newUrvalue < newAmpl) && (ri.type == actRngType)) {
            newAmpl = newUrvalue;
            p=i;
        }
    }

    if (p > -1) {
        return riList.at(p).name;
    }
    return getMaxRangeName(actRangeName); // we return maximum range in case of overload condition
}

QString cRangeMeasChannel::getMaxRangeName() const
{
    QList<cRangeInfoWithConstantValues> riList = m_rangeNameToChannelData.values();
    double newAmpl = -1.0;
    int foundRangeIdx = 0;
    for (int rangeIdx = 0; rangeIdx < riList.count(); rangeIdx++) {
        double newUrvalue = riList.at(rangeIdx).urvalue;
        if (newUrvalue > newAmpl) {
            newAmpl = newUrvalue;
            foundRangeIdx =rangeIdx;
        }
    }
    return riList.at(foundRangeIdx).name;
}

QString cRangeMeasChannel::getMaxRangeName(const QString &rangeName)
{
    qint32 actRngType = -1;
    if (m_rangeNameToChannelData.contains(rangeName)) {
        // if we know this rngname we take that's type for searching max range
        actRngType = m_rangeNameToChannelData[rangeName].type; // the type of actual range
    }
    else {
        // if we already set a range we could take the actual range for searching
        if (m_rangeNameToChannelData.contains(m_actRangeName)) {
            actRngType = m_rangeNameToChannelData[m_actRangeName].type;
        }
    }

    QList<cRangeInfoWithConstantValues> riList = m_rangeNameToChannelData.values();
    double newAmpl = -1.0;
    int rangeIdxFound = 0;
    for (int rangeIdx = 0; rangeIdx < riList.count(); rangeIdx++) {
        const cRangeInfoWithConstantValues& ri = riList.at(rangeIdx);
        double newUrvalue = ri.urvalue;
        if (actRngType >=0) {
            if ((newUrvalue > newAmpl) && (ri.type == actRngType)) {
                newAmpl = newUrvalue;
                rangeIdxFound = rangeIdx;
            }
        }
        else {
            if ((newUrvalue > newAmpl)) {
                newAmpl = newUrvalue;
                rangeIdxFound = rangeIdx;
            }
        }
    }
    return riList.at(rangeIdxFound).name;
}


QString cRangeMeasChannel::getSortedRangeNameList()
{
    return m_sortedRangeNameList;
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
    return ((m_statusBitMask & 1) > 0);
}

double cRangeMeasChannel::getUrValue(const QString &rangeName) const
{
    return m_rangeNameToChannelData[rangeName].urvalue;
}

double cRangeMeasChannel::getRejection(const QString &rangeName) const
{
    return m_rangeNameToChannelData[rangeName].rejection;
}

double cRangeMeasChannel::getOVRRejection(const QString &rangeName) const
{
    return m_rangeNameToChannelData[rangeName].ovrejection;
}


void cRangeMeasChannel::generateVeinInterface()
{
}

RangeChannelData *cRangeMeasChannel::getSharedChannelData()
{
    return &m_sharedChannelData;
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
        case unregisterNotifiers:
            if (reply == ack)
                emit deactivationContinue();
            else
                notifyError(unregisterpcbnotifierErrMsg);
            break;

        case readrangelist:
            if (reply == ack) {
                m_allRangeNamesOrderedByServer = answer.toStringList();
                emit activationContinue();
            }
            else
                notifyError(readrangelistErrMsg);
            break;

        case readtype:
            if (reply == ack) {
                m_oneWorkRangeInfo.type = answer.toInt(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangetypeErrMsg);
            break;

        case readurvalue:
            if (reply == ack) {
                m_oneWorkRangeInfo.urvalue = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeurvalueErrMsg);
            break;

        case readrejection:
            if (reply == ack) {
                m_oneWorkRangeInfo.rejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangerejectionErrMsg);
            break;

        case readovrejection:
            if (reply == ack) {
                m_oneWorkRangeInfo.ovrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeovrejectionErrMsg);
            break;

        case readadcrejection:
            if (reply == ack) {
                m_oneWorkRangeInfo.adcrejection = answer.toDouble(&ok);
                emit activationContinue();
            }
            else
                notifyError(readrangeadcrejectionErrMsg);
            break;

        case readisavail:
            if (reply == ack) {
                m_oneWorkRangeInfo.avail = answer.toBool();
                emit activationContinue();
            }
            else
                notifyError(readrangeavailErrMsg);
            break;

        case setmeaschannelrange:
            if (reply == ack)
                m_actRangeName = m_nextRangeName;
            else {
                qWarning("Set range %s on %s failed with %i!",
                         qPrintable(m_nextRangeName),
                         qPrintable(getAlias()), // channel alias
                         reply);
                notifyError(setRangeErrMsg);
            } // perhaps some error output
            emit cmdDone(msgnr);
            break;

        case readgaincorrectioninternalandclamp:
            if (reply == ack)
                m_fGainCorrection = answer.toDouble(&ok);
            else
                notifyError(readGainCorrErrMsg);
            emit cmdDone(msgnr);
            break;

        case readoffsetcorrection:
            if (reply == ack)
                m_fOffsetCorrection = answer.toDouble(&ok);
            else
                notifyError(readOffsetCorrErrMsg);
            emit cmdDone(msgnr);
            break;

        case readphasecorrection:
            if (reply == ack)
                m_fPhaseCorrection = answer.toDouble(&ok);
            else
                notifyError(readPhaseCorrErrMsg);
            emit cmdDone(msgnr);
            break;

        case readchannelstatusbitmask:
            if (reply == ack)
                m_statusBitMask = answer.toInt(&ok);
            else
                notifyError(readChannelStatusErrMsg);
            emit cmdDone(msgnr);
            break;

        case resetchannelstatusbitmask_emitcmddone:
            if (reply == ack) { }
            else
                notifyError(resetChannelStatusErrMsg);
            emit cmdDone(msgnr);
            break;

        case resetchannelstatusbitmask_instatemachine:
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

void cRangeMeasChannel::setSortedRangeNameList()
{
    QString s;
    QList<cRangeInfoWithConstantValues> riList = m_rangeNameToChannelData.values();
    int riLen;

    if ( (riLen = riList.count()) > 1) {// nothing to sort if we only have 1 range
        // first we sort the ranges according to range type
        for (int i = 0; i < riLen-1; i++) {
            for (int j = i; j < riLen; j++) {
                if (riList.at(i).type < riList.at(j).type) {
                    riList.swapItemsAt(i, j);
                }
            }
        }

        // second we sort the ranges according to upper range values but grouped with types
        for (int i = 0; i < riLen-1; i++) {
            for (int j = i; j < riLen; j++) {
                if ( (riList.at(i).urvalue < riList.at(j).urvalue) && (riList.at(i).type == riList.at(j).type) ) {
                    riList.swapItemsAt(i, j);
                }
            }
        }
    }

    s = riList.at(0).name;
    if (riLen > 1) {
        for (int i = 1; i < riList.count(); i++)
            s = s + ";" + riList.at(i).name;
    }

    m_sortedRangeNameList = s;
}

void cRangeMeasChannel::pcbConnection()
{
    m_pcbInterface->setClientSuperSmart(NetworkConnectionInfo(m_pcbNetworkInfo.m_sIP, m_pcbNetworkInfo.m_nPort),
                                                              m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbInterface->getClientSmart().get(), &Zera::ProxyClient::connected, &m_readRangeAndProperties);

    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cRangeMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbInterface->getClientSmart());
}


void cRangeMeasChannel::readRangeAndProperties()
{
    m_rangeQueryMachine.start(); // read all ranges and their properties
}


void cRangeMeasChannel::resetStatusSlot()
{
    m_MsgNrCmdList[m_pcbInterface->resetStatus(getMName())] = resetchannelstatusbitmask_instatemachine;
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
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_pcbInterface->getClientSmart());
    // and disconnect for our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

void cRangeMeasChannel::readRangelist()
{
    m_growingWorkRangeInfo.clear();
    m_MsgNrCmdList[m_pcbInterface->getRangeList(getMName())] = readrangelist;
    m_RangeQueryIt = 0; // we start with range 0
}

void cRangeMeasChannel::readType()
{
    m_MsgNrCmdList[m_pcbInterface->getType(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readtype;
}

void cRangeMeasChannel::readUrvalue()
{
    m_MsgNrCmdList[m_pcbInterface->getUrvalue(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readurvalue;
}


void cRangeMeasChannel::readRejection()
{
   m_MsgNrCmdList[m_pcbInterface->getRejection(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readrejection;
}


void cRangeMeasChannel::readOVRejection()
{
    m_MsgNrCmdList[m_pcbInterface->getOVRejection(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readovrejection;
}


void cRangeMeasChannel::readADCRejection()
{
    m_MsgNrCmdList[m_pcbInterface->getADCRejection(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readadcrejection;
}


void cRangeMeasChannel::readisAvail()
{
    m_MsgNrCmdList[m_pcbInterface->isAvail(getMName(), m_allRangeNamesOrderedByServer.at(m_RangeQueryIt))] = readisavail;
}


void cRangeMeasChannel::rangeQueryLoop()
{
    m_oneWorkRangeInfo.name = m_allRangeNamesOrderedByServer.at(m_RangeQueryIt);
    m_growingWorkRangeInfo[m_oneWorkRangeInfo.name] = m_oneWorkRangeInfo;

    m_RangeQueryIt++;
    if (m_RangeQueryIt < m_allRangeNamesOrderedByServer.count()) { // another range ?
        emit activationLoop();
    }
    else {
        QHash<QString, cRangeInfoWithConstantValues>::iterator it = m_growingWorkRangeInfo.begin();
        while (it != m_growingWorkRangeInfo.end()) { // we delete all unused ranges
            m_oneWorkRangeInfo= it.value();
            if (m_oneWorkRangeInfo.avail) {
                ++it;
            }
            else {
                it = m_growingWorkRangeInfo.erase(it); // in case range is not avail
            }
        }

        m_rangeNameToChannelData = m_growingWorkRangeInfo;

        setSortedRangeNameList(); // and the list of possible ranges
        emit activationContinue();
    }
}


}
