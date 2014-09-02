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

namespace DFTMODULE
{

struct cMeasChannelInfo
{
    quint8 dspChannelNr;
    QString alias;
    QString unit;
    cSocket socket;
    Zera::Server::cPCBInterface* pcbIFace;
};

}
#endif // MEASCHANNELINFO_H
