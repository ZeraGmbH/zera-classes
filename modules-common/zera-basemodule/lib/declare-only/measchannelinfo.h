#ifndef MEASCHANNELINFO_H
#define MEASCHANNELINFO_H

#include <pcbinterface.h>
#include <QObject>
#include <QString>

struct cMeasChannelInfo
{
    double m_fUrValue; // the channels actual upper range value
    Zera::PcbInterfacePtr pcbIFace;
};

#endif // MEASCHANNELINFO_H
