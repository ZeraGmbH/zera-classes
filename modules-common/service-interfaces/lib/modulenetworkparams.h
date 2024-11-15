#ifndef MODULENETWORKPARAMS_H
#define MODULENETWORKPARAMS_H

#include "networkconnectioninfo.h"
#include <abstracttcpnetworkfactory.h>
#include <memory>

struct ModuleNetworkParams
{
    ModuleNetworkParams(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                        NetworkConnectionInfo pcbServiceConnectionInfo,
                        NetworkConnectionInfo dspServiceConnectionInfo,
                        NetworkConnectionInfo secServiceConnectionInfo,
                        NetworkConnectionInfo rmServiceConnectionInfo);
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    const NetworkConnectionInfo m_pcbServiceConnectionInfo;
    const NetworkConnectionInfo m_dspServiceConnectionInfo;
    const NetworkConnectionInfo m_secServiceConnectionInfo;
    const NetworkConnectionInfo m_rmServiceConnectionInfo;
};

typedef std::shared_ptr<ModuleNetworkParams> ModuleNetworkParamsPtr;

#endif // MODULENETWORKPARAMS_H
