#ifndef POWER2DSPCMDGENERATOR_H
#define POWER2DSPCMDGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power2DspCmdGenerator
{
public:
    static QStringList getCmdsMMode4LW(int dspSelectCode, MeasSystemChannels measChannelPairList, quint32 sampleCountPerSignalPeriod);
    static QStringList getCmdsSumAndAverage();
};

#endif // POWER2DSPCMDGENERATOR_H
