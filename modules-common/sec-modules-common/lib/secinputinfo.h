#ifndef SECINPUTINFO_H
#define SECINPUTINFO_H

#include <pcbinterface.h>

struct SecInputInfo
{
    QString name; // the input's system name
    QString alias; // the input's alias
    QString resource; // the resource from which this input is served
    Zera::cPCBInterface* pcbIFace; // our interface to this server
};

#endif // SECINPUTINFO_H
