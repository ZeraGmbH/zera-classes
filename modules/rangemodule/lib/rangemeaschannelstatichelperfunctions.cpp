#include "rangemeaschannelstatichelperfunctions.h"

namespace RANGEMODULE
{

double RangeMeasChannelStaticHelperFunctions::getUrValueRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getUrValue(channel->getCurrRangeName());
}

double RangeMeasChannelStaticHelperFunctions::getRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getRejection(channel->getCurrRangeName());
}

double RangeMeasChannelStaticHelperFunctions::getOVRRejectionRangeAct(const cRangeMeasChannel *channel)
{
    return channel->getOVRRejection(channel->getCurrRangeName());
}

double RangeMeasChannelStaticHelperFunctions::getUrValueMaxRangeAct(const cRangeMeasChannel *channel)
{
    double actRangeUrValue = getUrValueRangeAct(channel);
    double actRangeRejection = getRejectionRangeAct(channel);
    double actRangeOVRRejection = getOVRRejectionRangeAct(channel);
    return (actRangeUrValue * actRangeOVRRejection / actRangeRejection);
}

double RangeMeasChannelStaticHelperFunctions::getUrValueMaxRangeMax(const cRangeMeasChannel *channel)
{
    QString maxRangeName = channel->getMaxRangeName();
    double maxRangeUrValue = channel->getUrValue(maxRangeName);
    double maxRangeRejection = channel->getRejection(maxRangeName);
    double maxRangeOVRRejection = channel->getOVRRejection(maxRangeName);
    return (maxRangeUrValue * maxRangeOVRRejection / maxRangeRejection);
}

}
