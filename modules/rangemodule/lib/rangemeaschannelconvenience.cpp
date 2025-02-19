#include "rangemeaschannelconvenience.h"

namespace RANGEMODULE
{

double RangeMeasChannelConvenience::getUrValueActRange(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getRange());
}

}
