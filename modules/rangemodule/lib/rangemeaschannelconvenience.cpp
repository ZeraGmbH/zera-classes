#include "rangemeaschannelconvenience.h"

namespace RANGEMODULE
{

double RangeMeasChannelConvenience::getUrValueActRange(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getRange());
}

double RangeMeasChannelConvenience::getOVRRejectionActRange(const cRangeMeasChannel *channel)
{
    return channel->getOVRRejection(channel->getRange());
}

}
