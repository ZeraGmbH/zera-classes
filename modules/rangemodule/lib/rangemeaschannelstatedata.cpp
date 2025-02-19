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
