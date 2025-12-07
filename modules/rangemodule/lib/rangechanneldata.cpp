#include "rangechanneldata.h"

namespace RANGEMODULE
{

RangeChannelData::RangeChannelData()
{
}

double RangeChannelData::getPreScaling() const
{
    return m_preScaling;
}

void RangeChannelData::setPreScaling(double preScaling)
{
    m_preScaling = preScaling;
}

double RangeChannelData::getRmsValue() const
{
    return m_rmsValue;
}

void RangeChannelData::setRmsValue(double rmsValue)
{
    m_rmsValue = rmsValue;
}

double RangeChannelData::getSignalFrequency() const
{
    return m_signalFrequency;
}

void RangeChannelData::setSignalFrequency(double signalFrequency)
{
    m_signalFrequency = signalFrequency;
}

double RangeChannelData::getPeakValue() const
{
    return m_peakValue;
}

void RangeChannelData::setPeakValue(double peakValue)
{
    m_peakValue = peakValue;
}

double RangeChannelData::getPeakValueWithDc() const
{
    return m_peakValueWithDc;
}

void RangeChannelData::setPeakValueWithDc(double peakValueWithDc)
{
    m_peakValueWithDc = peakValueWithDc;
}

}
