#ifndef ECALCINFO_H
#define ECALCINFO_H

#include "socket.h"
#include <service-interfaces/secinterface.h>

struct cECalcChannelInfo
{
    QString name;
    cSocket secServersocket;
    Zera::Server::cSECInterface* secIFace;
};

#endif // ECALCINFO_H
