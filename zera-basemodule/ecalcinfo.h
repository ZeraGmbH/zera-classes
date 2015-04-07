#ifndef ECALCINFO_H
#define ECALCINFO_H


#include <QObject>
#include <QString>

#include "socket.h"

namespace Zera {
namespace Server {
    class cSECInterface;
}
}

struct cECalcChannelInfo
{
    QString name;
    cSocket secServersocket;
    Zera::Server::cSECInterface* secIFace;
};


#endif // ECALCINFO_H
