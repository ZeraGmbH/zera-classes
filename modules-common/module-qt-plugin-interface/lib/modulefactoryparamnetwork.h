#ifndef MODULEFACTORYPARAMNETWORK_H
#define MODULEFACTORYPARAMNETWORK_H

#include "socket.h"
#include <abstracttcpnetworkfactory.h>
#include <memory>

struct ModuleFactoryParamNetwork
{
    ModuleFactoryParamNetwork(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                              cSocket pcbServiceConnectionInfo,
                              cSocket dspServiceConnectionInfo,
                              cSocket rmServiceConnectionInfo);
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    const cSocket m_pcbServiceConnectionInfo;
    const cSocket m_dspServiceConnectionInfo;
    const cSocket m_rmServiceConnectionInfo;
};

typedef std::shared_ptr<ModuleFactoryParamNetwork> ModuleFactoryParamNetworkPtr;

#endif // MODULEFACTORYPARAMNETWORK_H
