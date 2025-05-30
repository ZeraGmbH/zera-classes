#include "demovaluesdsprange.h"
#include <cmath>

DemoValuesDspRange::DemoValuesDspRange(int channels) :
    m_channels(channels),
    m_peakValues(channels),
    m_rmsValues(channels),
    m_freqValue(1),
    m_dcValues(channels)
{
}

void DemoValuesDspRange::setRmsPeakDCValue(int channel, float rmsValue)
{
    float peakValue = rmsValue * M_SQRT2;
    m_peakValues[channel] = peakValue;
    m_rmsValues[channel] = rmsValue;
    m_dcValues[channel] = peakValue;
}

void DemoValuesDspRange::setRmsPeakDCValue(int channel, float rmsValue, float peakValue)
{
    m_peakValues[channel] = peakValue;
    m_rmsValues[channel] = rmsValue;
    m_dcValues[channel] = peakValue;
}

void DemoValuesDspRange::setFrequency(float frequency)
{
    m_freqValue[0] = frequency;
}

QVector<float> DemoValuesDspRange::getDspValues()
{
    return m_peakValues + m_rmsValues + m_freqValue + m_dcValues;
}
