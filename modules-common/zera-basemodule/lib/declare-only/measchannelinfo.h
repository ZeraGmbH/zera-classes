#ifndef MEASCHANNELINFO_H
#define MEASCHANNELINFO_H

#include "networkconnectioninfo.h"
#include <pcbinterface.h>
#include <QObject>
#include <QString>

struct cMeasChannelInfo
{
    double m_fUrValue; // the channels actual upper range value
    NetworkConnectionInfo pcbServersocket;
    Zera::cPCBInterface* pcbIFace;
};

#endif // MEASCHANNELINFO_H
