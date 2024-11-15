#ifndef PCBSERVICECONNECTION_H
#define PCBSERVICECONNECTION_H

#include "modulenetworkparams.h"
#include <pcbinterface.h>

class PcbServiceConnection
{
public:
    void setNetworkParams(ModuleNetworkParamsPtr networkParams);
    Zera::PcbInterfacePtr getInterface();
    Zera::ProxyClientPtr getClient();
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // PCBSERVICECONNECTION_H
