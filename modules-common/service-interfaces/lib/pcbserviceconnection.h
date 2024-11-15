#ifndef PCBSERVICECONNECTION_H
#define PCBSERVICECONNECTION_H

#include "modulenetworkparams.h"
#include <tasktemplate.h>
#include <pcbinterface.h>

class PcbServiceConnection
{
public:
    void setNetworkParams(ModuleNetworkParamsPtr networkParams);
    TaskTemplatePtr createConnectionTask();
    Zera::PcbInterfacePtr getInterface();
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // PCBSERVICECONNECTION_H
