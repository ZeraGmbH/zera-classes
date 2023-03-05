#ifndef POWER2DSPGENERATOR_H
#define POWER2DSPGENERATOR_H

#include "meassytemchannels.h"
#include <QStringList>

class Power2DspGenerator
{
public:
    QStringList mmodeAdd4LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, quint32 sampleCountPerSignalPeriod);
};

#endif // POWER2DSPGENERATOR_H
