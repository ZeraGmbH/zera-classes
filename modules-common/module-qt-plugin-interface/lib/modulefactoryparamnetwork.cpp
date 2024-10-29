#include "modulefactoryparamnetwork.h"

ModuleFactoryParamNetwork::ModuleFactoryParamNetwork(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                     NetworkConnectionInfo pcbServiceConnectionInfo,
                                                     NetworkConnectionInfo dspServiceConnectionInfo,
                                                     NetworkConnectionInfo rmServiceConnectionInfo) :
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_pcbServiceConnectionInfo(pcbServiceConnectionInfo),
    m_dspServiceConnectionInfo(dspServiceConnectionInfo),
    m_rmServiceConnectionInfo(rmServiceConnectionInfo)
{
}
