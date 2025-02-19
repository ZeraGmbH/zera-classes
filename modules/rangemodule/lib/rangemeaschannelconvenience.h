#ifndef RANGEMEASCHANNELCONVENIENCE_H
#define RANGEMEASCHANNELCONVENIENCE_H

#include "rangemeaschannel.h"

namespace RANGEMODULE
{

class RangeMeasChannelConvenience
{
public:
    static double getUrValueRangeAct(const cRangeMeasChannel *channel);
    static double getRejectionRangeAct(const cRangeMeasChannel *channel);
    static double getOVRRejectionRangeAct(const cRangeMeasChannel *channel);

    static double getUrValueMaxRangeAct(const cRangeMeasChannel *channel);
    static double getUrValueMaxRangeMax(const cRangeMeasChannel *channel);
};

}
#endif // RANGEMEASCHANNELCONVENIENCE_H
