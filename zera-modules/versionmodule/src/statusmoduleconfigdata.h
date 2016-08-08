#ifndef MODEMODULECONFIGDATA_H
#define MODEMODULECONFIGDATA_H

#include <QString>

#include "socket.h"

namespace MODEMODULE
{

class cModeModuleConfigData
{
public:
    cModeModuleConfigData(){}
    quint8 m_nDebugLevel;
    QString m_sMode;
    cSocket m_RMSocket; // the sockets we can connect to
    cSocket m_PCBServerSocket;
    cSocket m_DSPServerSocket;
};

}

#endif // MODEMODULECONFIGDATA_H
