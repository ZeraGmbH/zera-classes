#include "modulefactoryparamnetwork.h"

ModuleFactoryParamNetwork::ModuleFactoryParamNetwork(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                     cSocket pcbServiceConnectionInfo,
                                                     cSocket dspServiceConnectionInfo,
                                                     cSocket rmServiceConnectionInfo) :
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_pcbServiceConnectionInfo(pcbServiceConnectionInfo),
    m_dspServiceConnectionInfo(dspServiceConnectionInfo),
    m_rmServiceConnectionInfo(rmServiceConnectionInfo)
{
}
