#ifndef POWER2DSPGENERATOR_H
#define POWER2DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power2DspGenerator
{
public:
    QStringList getCmdsMMode4LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, quint32 sampleCountPerSignalPeriod);
    QStringList getCmdsSumAndAverage();
};

#endif // POWER2DSPGENERATOR_H
