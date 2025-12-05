#include "channeladjstorage.h"

double ChannelAdjStorage::getLastAdjAmplitude() const
{
    return m_lastAdjAmplitude;
}

void ChannelAdjStorage::setLastAdjAmplitude(double lastAdjAmplitude)
{
    m_lastAdjAmplitude = lastAdjAmplitude;
}

double ChannelAdjStorage::getLastGainCorrectionInternalAndClamp() const
{
    return m_lastGainCorrectionInternalAndClamp;
}

void ChannelAdjStorage::setLastGainCorrectionInternalAndClamp(double lastGainCorrectionInternalAndClamp)
{
    m_lastGainCorrectionInternalAndClamp = lastGainCorrectionInternalAndClamp;
}
