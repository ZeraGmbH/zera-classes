#ifndef POWER1DSPGENERATOR_H
#define POWER1DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power1DspGenerator
{
public:
    static QStringList getCmdsMMode4LW(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LB(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LS(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LSg(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode2LW(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMMode2LB(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMMode2LS(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMMode2LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMModeMQREF(int dspSelectCode);
    static QStringList getCmdsSumAndAverage();
};

#endif // POWER1DSPGENERATOR_H
