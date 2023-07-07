#ifndef STATUSMODULECONFIGDATA_H
#define STATUSMODULECONFIGDATA_H

#include <QString>

#include "socket.h"

namespace STATUSMODULE
{

class cStatusModuleConfigData
{
public:
    cStatusModuleConfigData(){}
    quint8 m_nDebugLevel;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
    bool m_accumulator;
};

}

#endif // STATUSMODULECONFIGDATA_H
