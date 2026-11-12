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
    m_resetStatusState.addTransition(this, &cRangeMeasChannel::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectionState);
    m_activationMachine.addState(&m_resetStatusState);
    m_activationMachine.addState(&m_activationDoneState);

    connect(&m_pcbConnectionState, &QState::entered, this, &cRangeMeasChannel::pcbConnection);
    connect(&m_resetStatusState, &QState::entered, this, &cRangeMeasChannel::resetStatusSlot);
    connect(&m_activationDoneState, &QState::entered, this, &cRangeMeasChannel::activationDone);

    // setting up statemachine for "deactivating" meas channel
    m_deactivationMachine.addState(&m_deactivationDoneState);

    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeMeasChannel::deactivationDone);

    m_activationMachine.setInitialState(&m_pcbConnectionState);
    m_deactivationMachine.setInitialState(&m_deactivationDoneState);

    connect(channelObserver.get(), &ChannelRangeObserver::Channel::sigNewRanges, this, &cRangeMeasChannel::newRangeList);
}

enum rangemeaschannelCmds
{
    setmeaschannelrange,
    readgaincorrectioninternalandclamp,
    readoffsetcorrection,
    readphasecorrection,
    readchannelstatusbitmask,
    resetchannelstatusbitmask_emitcmddone,
    resetchannelstatusbitmask_instatemachine,
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
    return 1;
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
    return 1;
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
    return 1;
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
    return 1;
}

quint32 cRangeMeasChannel::resetStatus()
{
    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->resetStatus(getMName());
        m_MsgNrCmdList[msgnr] = resetchannelstatusbitmask_emitcmddone;
        return msgnr;
    }
    return 1;
}

quint32 cRangeMeasChannel::readPhaseCorrection(double frequency)
{
    if (m_bActive) {
        quint32 msgnr = m_pcbInterface->getPhaseCorrection(getMName(), m_actRangeName, frequency);
        m_MsgNrCmdList[msgnr] = readphasecorrection;
        return msgnr;
    }
    return 1;
}

bool cRangeMeasChannel::isPossibleRange(const QString &rangeName)
{
    return m_channelObserver->getAvailRangeNames().contains(rangeName);
}

bool cRangeMeasChannel::isRMSOverload(double ampl)
{
    ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(m_actRangeName);
    double ovrRejectionFactor = range->getOvrRejection() / range->getRejection();
    return ((range->getUrValue() * ovrRejectionFactor) < ampl);
}

bool cRangeMeasChannel::isPeakOverload(double peak)
{
    ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(m_actRangeName);
    double ovrRejectionFactor = range->getOvrRejection() / range->getRejection();
    double overloadlimit = range->getUrValue() * ovrRejectionFactor * M_SQRT2;
    return (overloadlimit < peak);
}

QString cRangeMeasChannel::getOptimalRange(double rms, double peak, const QString &rangeName)
{
    qint32 suggestedRangeType = suggestTypeForNextRange(rangeName);
    if (suggestedRangeType < 0)
        suggestedRangeType = 0;

    QString newRangeRmsStr = findOptimalRange(rms, rangeName, suggestedRangeType, 1);
    double newRangeRmsUrValue = m_channelObserver->getRange(newRangeRmsStr)->getUrValue();
    QString newRangePeakStr = findOptimalRange(peak, rangeName, suggestedRangeType, M_SQRT2);
    double newRangePeakUrValue = m_channelObserver->getRange(newRangePeakStr)->getUrValue();

    return newRangePeakUrValue > newRangeRmsUrValue ? newRangePeakStr : newRangeRmsStr;
}

QString cRangeMeasChannel::findOptimalRange(double actualValue, const QString &actRangeName, qint32 suggestedRangeType, double extraRejectionFactor)
{
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

    const QStringList availRanges = m_channelObserver->getAvailRangeNames();
    double newAmpl = 1e32;
    int preferredRangeIdx = -1;

    for (int rangeIdx=0; rangeIdx<availRanges.count(); ++rangeIdx) {
        const QString &rangeName = availRanges[rangeIdx];
        ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(rangeName);
        const double newUrvalue = range->getUrValue();
        double ovrRejectionFactor = (range->getOvrRejection() / range->getRejection()) * extraRejectionFactor; // typically 1.25
        // actual range?
        if(actRangeName == rangeName) {
            // are we in hysteresis area?
            if(actualValue > newUrvalue * ovrRejectionFactor * enterRangeLimit &&
                actualValue < newUrvalue * ovrRejectionFactor * keepRangeLimit) {
                // let's keep actual range
                preferredRangeIdx = rangeIdx;
                break;
            }
        }
        // (re-)enter range
        if ((actualValue <= newUrvalue * ovrRejectionFactor * enterRangeLimit) && (newUrvalue < newAmpl) && (range->getType() == suggestedRangeType)) {
            newAmpl = newUrvalue;
            preferredRangeIdx = rangeIdx;
        }
    }
    if (preferredRangeIdx > -1)
        return availRanges[preferredRangeIdx];

    return getMaxRangeName(actRangeName); // we return maximum range in case of overload condition
}

qint32 cRangeMeasChannel::suggestTypeForNextRange(const QString &rangeName)
{
    qint32 suggestedType = -1;
    // if we know this rngname we take it's type for searching max range
    if (isPossibleRange(rangeName))
        suggestedType = m_channelObserver->getRange(rangeName)->getType();

    // if we already set a range we could take the actual range for searching
    else if (isPossibleRange(m_actRangeName))
        suggestedType = m_channelObserver->getRange(m_actRangeName)->getType();
    return suggestedType;
}

QString cRangeMeasChannel::getMaxRangeName() const
{
    const QStringList availRanges = m_channelObserver->getAvailRangeNames();
    double maxUrValue = -1.0;
    int foundRangeIdx = 0;

    for (int rangeIdx=0; rangeIdx<availRanges.count(); ++rangeIdx) {
        const QString &rangeName = availRanges[rangeIdx];
        ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(rangeName);
        const double newUrvalue = range->getUrValue();
        if (newUrvalue > maxUrValue) {
            maxUrValue = newUrvalue;
            foundRangeIdx = rangeIdx;
        }
    }
    return availRanges[foundRangeIdx];
}

QString cRangeMeasChannel::getMaxRangeName(const QString &rangeName)
{
    qint32 suggestedRangeType = suggestTypeForNextRange(rangeName);

    const QStringList availRanges = m_channelObserver->getAvailRangeNames();
    double maxUrValue = -1.0;
    int rangeIdxFound = 0;

    for (int rangeIdx = 0; rangeIdx < availRanges.count(); rangeIdx++) {
        const QString &currRange = availRanges[rangeIdx];
        ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(currRange);
        double newUrvalue = range->getUrValue();
        if (suggestedRangeType >= 0) {
            if ((newUrvalue > maxUrValue) && (range->getType() == suggestedRangeType)) {
                maxUrValue = newUrvalue;
                rangeIdxFound = rangeIdx;
            }
        }
        else {
            if ((newUrvalue > maxUrValue)) {
                maxUrValue = newUrvalue;
                rangeIdxFound = rangeIdx;
            }
        }
    }
    return availRanges[rangeIdxFound];
}

QString cRangeMeasChannel::getAvailAliasList()
{
    return m_channelObserver->getAvailAliasList();
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
    ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(rangeName);
    return range->getUrValue();
}

double cRangeMeasChannel::getRejection(const QString &rangeName) const
{
    ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(rangeName);
    return range->getRejection();
}

double cRangeMeasChannel::getOVRRejection(const QString &rangeName) const
{
    ChannelRangeObserver::RangePtr range = m_channelObserver->getRange(rangeName);
    return range->getOvrRejection();
}

RangeChannelData *cRangeMeasChannel::getSharedChannelData()
{
    return &m_sharedChannelData;
}

void cRangeMeasChannel::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) {// 0 was reserved for async. messages
    }
    else {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd)
        {
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
        case readchannelstatusbitmask:
            if (reply == ack)
                m_statusBitMask = answer.toInt();
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
        }
    }
}

void cRangeMeasChannel::pcbConnection()
{
    m_pcbInterface->setClientSuperSmart(NetworkConnectionInfo(m_pcbNetworkInfo.m_sIP, m_pcbNetworkInfo.m_nPort),
                                                              m_tcpNetworkFactory);
    m_pcbConnectionState.addTransition(m_pcbInterface->getClientSmart().get(), &Zera::ProxyClient::connected, &m_resetStatusState);

    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer, this, &cRangeMeasChannel::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbInterface->getClientSmart());
}

void cRangeMeasChannel::resetStatusSlot()
{
    m_MsgNrCmdList[m_pcbInterface->resetStatus(getMName())] = resetchannelstatusbitmask_instatemachine;
}

void cRangeMeasChannel::activationDone()
{
    m_bActive = true;
    emit activated();
}

void cRangeMeasChannel::deactivationDone()
{
    Zera::Proxy::getInstance()->releaseConnectionSmart(m_pcbInterface->getClientSmart());
    // and disconnect for our servers afterwards
    disconnect(m_pcbInterface.get(), 0, this, 0);
    emit deactivated();
}

}
