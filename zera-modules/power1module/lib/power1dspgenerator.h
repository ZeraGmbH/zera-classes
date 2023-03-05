#ifndef POWER1DSPGENERATOR_H
#define POWER1DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power1DspGenerator
{
public:
    QStringList getCmdsMMode4LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode4LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode4LBK(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode4LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode4LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode3LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode3LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMMode2LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList getCmdsMMode2LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList getCmdsMMode2LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList getCmdsMMode2LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList getCmdsMModeXLW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList getCmdsMModeMQREF(int dspSelectCode);
    QStringList getCmdsSumAndAverage();
};

#endif // POWER1DSPGENERATOR_H
