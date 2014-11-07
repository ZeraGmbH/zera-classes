#ifndef MEASCHANNELINFO_H
#define MEASCHANNELINFO_H

#include <QObject>
#include <QString>

#include "socket.h"

namespace Zera {
namespace Server {
    class cPCBInterface;
}
}

struct cMeasChannelInfo
{
    quint8 dspChannelNr;
    double m_fUrValue; // the channels actual upper range value
    QString alias;
    QString unit;
    cSocket pcbServersocket;
    Zera::Server::cPCBInterface* pcbIFace;
};

#endif // MEASCHANNELINFO_H
