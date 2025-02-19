#include "rangemeaschannelconvenience.h"

namespace RANGEMODULE
{

double RangeMeasChannelConvenience::getUrValueRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getRange());
}

double RangeMeasChannelConvenience::getRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getRejection(channel->getRange());
}

double RangeMeasChannelConvenience::getOVRRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getOVRRejection(channel->getRange());
}

double RangeMeasChannelConvenience::getUrValueMaxRangeAct(const cRangeMeasChannel *channel)
{
    double actRangeUrValue = getUrValueRangeAct(channel);
    double actRangeRejection = getRejectionRangeAct(channel);
    double actRangeOVRRejection = getOVRRejectionRangeAct(channel);
    return (actRangeUrValue * actRangeOVRRejection / actRangeRejection);
}

double RangeMeasChannelConvenience::getUrValueMaxRangeMax(const cRangeMeasChannel *channel)
{
    QString maxRange = channel->getMaxRange();
    double maxRangeUrValue = channel->getUrValue(maxRange);
    double maxRangeRejection = channel->getRejection(maxRange);
    double maxRangeOVRRejection = channel->getOVRRejection(maxRange);
    return (maxRangeUrValue * maxRangeOVRRejection / maxRangeRejection);
}

}
