#include "pcbserviceconnection.h"
#include "taskserverconnectionstart.h"
#include <proxy.h>

PcbServiceConnection::PcbServiceConnection(const NetworkConnectionInfo &networkInfo,
                                           VeinTcp::AbstractTcpNetworkFactoryPtr networkFactory)
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(networkInfo,
                                                                 networkFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
}

PcbServiceConnection::PcbServiceConnection(ModuleNetworkParamsPtr networkParams) :
    PcbServiceConnection(networkParams->m_pcbServiceConnectionInfo,
                         networkParams->m_tcpNetworkFactory)
{
}

TaskTemplatePtr PcbServiceConnection::createConnectionTask() const
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

Zera::PcbInterfacePtr PcbServiceConnection::getInterface() const
{
    return m_pcbInterface;
}
