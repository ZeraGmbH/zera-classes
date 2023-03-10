#ifndef POWER1DSPCMDGENERATOR_H
#define POWER1DSPCMDGENERATOR_H

#include "power1moduleconfigdata.h"
#include "meassytemchannels.h"
#include <QStringList>

class Power1DspCmdGenerator
{
public:
    static QStringList getCmdsMModeMQREF(int dspSelectCode);
    static QStringList getCmdsMMode4LB(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LBK(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LS(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode4LSg(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode3LW(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode3LB(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMMode2LB(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMMode2LS(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMMode2LSg(int dspSelectCode, MeasSystemChannels measChannelPairList, int idx2WireMeasSystem);
    static QStringList getCmdsMModeXLW(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsMModeXLB(int dspSelectCode, MeasSystemChannels measChannelPairList);
    static QStringList getCmdsSumAndAverage();
    static QStringList getCmdsFreqOutput(const POWER1MODULE::cPower1ModuleConfigData *configData,
                                         const QHash<QString, cFoutInfo> foutInfoHash,
                                         int irqNo);
};

#endif // POWER1DSPCMDGENERATOR_H
