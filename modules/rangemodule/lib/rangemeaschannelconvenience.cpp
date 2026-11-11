#include "rangemeaschannelconvenience.h"

namespace RANGEMODULE
{

double RangeMeasChannelConvenience::getUrValueRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getCurrRangeName());
}

double RangeMeasChannelConvenience::getRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getRejection(channel->getCurrRangeName());
}

double RangeMeasChannelConvenience::getOVRRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getOVRRejection(channel->getCurrRangeName());
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
    QString maxRangeName = channel->getMaxRangeName();
    double maxRangeUrValue = channel->getUrValue(maxRangeName);
    double maxRangeRejection = channel->getRejection(maxRangeName);
    double maxRangeOVRRejection = channel->getOVRRejection(maxRangeName);
    return (maxRangeUrValue * maxRangeOVRRejection / maxRangeRejection);
}

}
