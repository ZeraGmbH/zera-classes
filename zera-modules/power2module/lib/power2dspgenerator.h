#ifndef POWER2DSPGENERATOR_H
#define POWER2DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power2DspGenerator
{
public:
    static QStringList getCmdsMMode4LW(int dspSelectCode, MeasSystemChannels measChannelPairList, quint32 sampleCountPerSignalPeriod);
    static QStringList getCmdsSumAndAverage();
};

#endif // POWER2DSPGENERATOR_H
