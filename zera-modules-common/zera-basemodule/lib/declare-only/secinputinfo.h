#ifndef SECINPUTINFO_H
#define SECINPUTINFO_H

#include "socket.h"
#include <pcbinterface.h>

struct cSecInputInfo
{
    QString name; // the input's system name
    QString alias; // the input's alias
    QString resource; // the resource from which this input is served
    cSocket pcbServersocket; // which pcbserver provides this input
    Zera::cPCBInterface* pcbIFace; // our interface to this server
};

#endif // SECINPUTINFO_H
