#ifndef POWER1DSPGENERATOR_H
#define POWER1DSPGENERATOR_H

#include <QStringList>

struct MeasSystemChannels
{
    int voltageChannel;
    int currentChannel;
};

class Power1DspGenerator
{
public:
    QStringList mmodeAdd4LW(int dspSelectCode);
    QStringList mmodeAdd4LB(int dspSelectCode);
    QStringList mmodeAdd4LBK(int dspSelectCode);
    QStringList mmodeAdd4LS(int dspSelectCode);
    QStringList mmodeAdd4LSg(int dspSelectCode);
    QStringList mmodeAdd3LW(int dspSelectCode);
    QStringList mmodeAdd3LB(int dspSelectCode);
    QStringList mmodeAdd2LW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LB(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LS(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAdd2LSg(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList, int idx2WireMeasSystem);
    QStringList mmodeAddXLW(int dspSelectCode, QList<MeasSystemChannels> measChannelPairList);
    QStringList mmodeAddMQREF(int dspSelectCode);
};

#endif // POWER1DSPGENERATOR_H
