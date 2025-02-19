#include "rangemeaschannelstatedata.h"

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
