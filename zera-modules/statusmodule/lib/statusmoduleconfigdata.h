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
    bool m_bDemo;
    quint8 m_nDebugLevel;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}

#endif // STATUSMODULECONFIGDATA_H
