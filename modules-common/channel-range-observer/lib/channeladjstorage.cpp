#include "channeladjstorage.h"

double ChannelAdjStorage::getLastGainAdjAmplitude() const
{
    return m_lastGainAdjAmplitude;
}

void ChannelAdjStorage::setLastGainAdjAmplitude(double lastGainAdjAmplitude)
{
    m_lastGainAdjAmplitude = lastGainAdjAmplitude;
}

double ChannelAdjStorage::getLastOffsetAdjAmplitude() const
{
    return m_lastOffsetAdjAmplitude;
}

void ChannelAdjStorage::setLastOffsetAdjAmplitude(double lastOffsetAdjAmplitude)
{
    m_lastOffsetAdjAmplitude = lastOffsetAdjAmplitude;
}
