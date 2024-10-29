#ifndef MODULEFACTORYPARAMNETWORK_H
#define MODULEFACTORYPARAMNETWORK_H

#include "networkconnectioninfo.h"
#include <abstracttcpnetworkfactory.h>
#include <memory>

struct ModuleFactoryParamNetwork
{
    ModuleFactoryParamNetwork(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                              NetworkConnectionInfo pcbServiceConnectionInfo,
                              NetworkConnectionInfo dspServiceConnectionInfo,
                              NetworkConnectionInfo rmServiceConnectionInfo);
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    const NetworkConnectionInfo m_pcbServiceConnectionInfo;
    const NetworkConnectionInfo m_dspServiceConnectionInfo;
    const NetworkConnectionInfo m_rmServiceConnectionInfo;
};

typedef std::shared_ptr<ModuleFactoryParamNetwork> ModuleFactoryParamNetworkPtr;

#endif // MODULEFACTORYPARAMNETWORK_H
