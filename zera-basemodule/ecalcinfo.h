#ifndef ECALCINFO_H
#define ECALCINFO_H

#include "socket.h"
#include <secinterface.h>

struct cECalcChannelInfo
{
    QString name;
    cSocket secServersocket;
    Zera::Server::cSECInterface* secIFace;
};

#endif // ECALCINFO_H
