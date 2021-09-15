#ifndef SOURCEMODULECONFIGDATA_H
#define SOURCEMODULECONFIGDATA_H

#include <QString>

#include "socket.h"

namespace SOURCEMODULE
{

class cSourceModuleConfigData
{
public:
    cSourceModuleConfigData(){}
    quint8 m_nDebugLevel;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}

#endif // SOURCEMODULECONFIGDATA_H
