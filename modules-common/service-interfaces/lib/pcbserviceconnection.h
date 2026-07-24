#ifndef PCBSERVICECONNECTION_H
#define PCBSERVICECONNECTION_H

#include "modulenetworkparams.h"
#include <tasktemplate.h>
#include <pcbinterface.h>

class PcbServiceConnection
{
public:
    PcbServiceConnection(const NetworkConnectionInfo &networkInfo, const VeinTcp::AbstractTcpNetworkFactoryPtr &networkFactory);
    PcbServiceConnection(const ModuleNetworkParamsPtr &networkParams);
    TaskTemplatePtr createConnectionTask() const;
    Zera::PcbInterfacePtr getInterface() const;
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientPtr m_pcbClient;
};

#endif // PCBSERVICECONNECTION_H
