#ifndef POWER1DSPGENERATOR_H
#define POWER1DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power1DspGenerator
{
public:
    QStringList mmodeAdd4LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd4LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd4LBK(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd4LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd4LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd3LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd3LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAdd2LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAddXLW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAddMQREF(int dspSelectCode);
    QStringList getCmdsSumAndAverage();
};

#endif // POWER1DSPGENERATOR_H
