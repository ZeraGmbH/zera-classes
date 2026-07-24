#include "modulenetworkparams.h"

ModuleNetworkParams::ModuleNetworkParams(const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                                         const NetworkConnectionInfo &pcbServiceConnectionInfo,
                                         const NetworkConnectionInfo &dspServiceConnectionInfo,
                                         const NetworkConnectionInfo &secServiceConnectionInfo,
                                         const NetworkConnectionInfo &rmServiceConnectionInfo) :
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_pcbServiceConnectionInfo(pcbServiceConnectionInfo),
    m_dspServiceConnectionInfo(dspServiceConnectionInfo),
    m_secServiceConnectionInfo(secServiceConnectionInfo),
    m_rmServiceConnectionInfo(rmServiceConnectionInfo)
{
}
