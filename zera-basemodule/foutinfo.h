#ifndef FOUTINFO_H
#define FOUTINFO_H

#include "socket.h"
#include <service-interfaces/pcbinterface.h>

enum foutmodes
{
    absPower, posPower, negPower
};

struct cFoutInfo
{
    QString name; // the system name
    QString alias; // the alias name example: system name fo0 alias FPZ1
    qint32 type; // the fout channels type (future purpose)
    bool avail; // may be it is not available (future purpose)
    quint8 dspFoutChannel; // the dspchannel that is responsible for this output 0..3
    double formFactor;
    cSocket dspServersocket; // in case that different dsp's drive frequency outputs (future purpose)
    cSocket pcbServersocket;
    Zera::Server::cPCBInterface* pcbIFace;
};

#endif // FOUTINFO_H
