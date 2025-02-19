#include "rangemeaschannelconvenience.h"

namespace RANGEMODULE
{

double RangeMeasChannelConvenience::getUrValueActRange(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getRange());
}

double RangeMeasChannelConvenience::getRejectionActRange(const cRangeMeasChannel *channel)
{
    return channel->getRejection(channel->getRange());
}

double RangeMeasChannelConvenience::getOVRRejectionActRange(const cRangeMeasChannel *channel)
{
    return channel->getOVRRejection(channel->getRange());
}

double RangeMeasChannelConvenience::getUrValueMaxActRange(const cRangeMeasChannel *channel)
{
    double actRangeUrValue = getUrValueActRange(channel);
    double actRangeRejection = getRejectionActRange(channel);
    double actRangeOVRRejection = getOVRRejectionActRange(channel);
    return (actRangeUrValue * actRangeOVRRejection / actRangeRejection);
}

double RangeMeasChannelConvenience::getUrValueMaxMaxRange(const cRangeMeasChannel *channel)
{
    QString maxRange = channel->getMaxRange();
    double maxRangeUrValue = channel->getUrValue(maxRange);
    double maxRangeRejection = channel->getRejection(maxRange);
    double maxRangeOVRRejection = channel->getOVRRejection(maxRange);
    return (maxRangeUrValue * maxRangeOVRRejection / maxRangeRejection);
}

}
