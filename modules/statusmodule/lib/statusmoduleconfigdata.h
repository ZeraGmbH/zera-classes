#ifndef STATUSMODULECONFIGDATA_H
#define STATUSMODULECONFIGDATA_H

#include <QString>

#include "networkconnectioninfo.h"

namespace STATUSMODULE
{

class cStatusModuleConfigData
{
public:
    cStatusModuleConfigData(){}
    NetworkConnectionInfo m_RMSocket; // the sockets we can connect to
    NetworkConnectionInfo m_PCBServerSocket;
    NetworkConnectionInfo m_DSPServerSocket;
    bool m_accumulator;
};

}

#endif // STATUSMODULECONFIGDATA_H
