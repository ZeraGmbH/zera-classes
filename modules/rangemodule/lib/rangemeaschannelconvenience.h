#ifndef RANGEMEASCHANNELCONVENIENCE_H
#define RANGEMEASCHANNELCONVENIENCE_H

#include "rangemeaschannel.h"

namespace RANGEMODULE
{

class RangeMeasChannelConvenience
{
public:
    static double getUrValueActRange(const cRangeMeasChannel *channel);
    static double getRejectionActRange(const cRangeMeasChannel *channel);
    static double getOVRRejectionActRange(const cRangeMeasChannel *channel);
};

}
#endif // RANGEMEASCHANNELCONVENIENCE_H
