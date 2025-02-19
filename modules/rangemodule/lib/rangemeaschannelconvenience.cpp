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
    double actUrValue = getUrValueActRange(channel);
    double actRejection = getRejectionActRange(channel);
    double actOVRRejection = getOVRRejectionActRange(channel);
    return (actUrValue * actOVRRejection / actRejection);
}

}
