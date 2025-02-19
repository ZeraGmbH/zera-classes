#ifndef RANGEMEASCHANNELCONVENIENCE_H
#define RANGEMEASCHANNELCONVENIENCE_H

#include "rangemeaschannel.h"

namespace RANGEMODULE
{

class RangeMeasChannelConvenience
{
public:
    static double getUrValueActRange(const cRangeMeasChannel *channel); // returns upper range of actual range
};

}
#endif // RANGEMEASCHANNELCONVENIENCE_H
