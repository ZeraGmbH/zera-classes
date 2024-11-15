#include "modulefactoryparamnetwork.h"

ModuleFactoryParamNetwork::ModuleFactoryParamNetwork(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                     NetworkConnectionInfo pcbServiceConnectionInfo,
                                                     NetworkConnectionInfo dspServiceConnectionInfo,
                                                     NetworkConnectionInfo secServiceConnectionInfo,
                                                     NetworkConnectionInfo rmServiceConnectionInfo) :
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_pcbServiceConnectionInfo(pcbServiceConnectionInfo),
    m_dspServiceConnectionInfo(dspServiceConnectionInfo),
    m_secServiceConnectionInfo(secServiceConnectionInfo),
    m_rmServiceConnectionInfo(rmServiceConnectionInfo)
{
}
