#ifndef MEASSYTEMCHANNELS_H
#define MEASSYTEMCHANNELS_H

#include <QVector>

struct MeasSystemChannel
{
    int voltageChannel;
    int currentChannel;
};

typedef QVector<MeasSystemChannel> MeasSystemChannels;

#endif // MEASSYTEMCHANNELS_H
