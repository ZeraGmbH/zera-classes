#include "channelcommonstorage.h"

bool ChannelCommonStorage::getInvertedPhaseState() const
{
    return m_invertPhase;
}

void ChannelCommonStorage::setInvertedPhaseState(bool inverted)
{
    m_invertPhase = inverted;
}

double ChannelCommonStorage::getLastGainAdjAmplitude() const
{
    return m_lastGainAdjAmplitude;
}

void ChannelCommonStorage::setLastGainAdjAmplitude(double lastGainAdjAmplitude)
{
    m_lastGainAdjAmplitude = lastGainAdjAmplitude;
}

double ChannelCommonStorage::getLastOffsetAdjAmplitude() const
{
    return m_lastOffsetAdjAmplitude;
}

void ChannelCommonStorage::setLastOffsetAdjAmplitude(double lastOffsetAdjAmplitude)
{
    m_lastOffsetAdjAmplitude = lastOffsetAdjAmplitude;
}
