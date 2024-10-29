#ifndef MEASCHANNELINFO_H
#define MEASCHANNELINFO_H

#include "networkconnectioninfo.h"
#include <pcbinterface.h>
#include <QObject>
#include <QString>

struct cMeasChannelInfo
{
    quint8 dspChannelNr;
    double m_fUrValue; // the channels actual upper range value
    QString alias;
    QString unit;
    NetworkConnectionInfo pcbServersocket;
    Zera::cPCBInterface* pcbIFace;
};

#endif // MEASCHANNELINFO_H
