#include "rangemeaschannelstatedata.h"

namespace RANGEMODULE
{

RangeMeasChannelStateData::RangeMeasChannelStateData()
{
}

bool RangeMeasChannelStateData::getInvertedPhaseState() const
{
    return m_invertedPhase;
}

void RangeMeasChannelStateData::setInvertedPhaseState(bool inverted)
{
    m_invertedPhase = inverted;
}

double RangeMeasChannelStateData::getPreScaling() const
{
    return m_preScaling;
}

void RangeMeasChannelStateData::setPreScaling(double preScaling)
{
    m_preScaling = preScaling;
}

double RangeMeasChannelStateData::getRmsValue() const
{
    return m_rmsValue;
}

void RangeMeasChannelStateData::setRmsValue(double rmsValue)
{
    m_rmsValue = rmsValue;
}

double RangeMeasChannelStateData::getSignalFrequency() const
{
    return m_signalFrequency;
}

void RangeMeasChannelStateData::setSignalFrequency(double signalFrequency)
{
    m_signalFrequency = signalFrequency;
}

double RangeMeasChannelStateData::getPeakValue() const
{
    return m_peakValue;
}

void RangeMeasChannelStateData::setPeakValue(double peakValue)
{
    m_peakValue = peakValue;
}

double RangeMeasChannelStateData::getPeakValueWithDc() const
{
    return m_peakValueWithDc;
}

void RangeMeasChannelStateData::setPeakValueWithDc(double peakValueWithDc)
{
    m_peakValueWithDc = peakValueWithDc;
}

}
