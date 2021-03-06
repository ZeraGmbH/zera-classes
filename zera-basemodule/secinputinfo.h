#ifndef SECINPUTINFO_H
#define SECINPUTINFO_H


#include <QObject>
#include <QString>

#include "socket.h"

namespace Zera {
namespace Server {
    class cPCBInterface;
}
}

struct cSecInputInfo
{
    QString name; // the input's system name
    QString alias; // the input's alias
    QString resource; // the resource from which this input is served
    cSocket pcbServersocket; // which pcbserver provides this input
    Zera::Server::cPCBInterface* pcbIFace; // our interface to this server
};

#endif // SECINPUTINFO_H
