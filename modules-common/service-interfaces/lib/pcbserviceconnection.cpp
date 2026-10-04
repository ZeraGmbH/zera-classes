#include "pcbserviceconnection.h"
#include "taskserverconnectionstart.h"
#include <proxy.h>

PcbServiceConnection::PcbServiceConnection(const NetworkConnectionInfo &networkInfo,
                                           const VeinTcp::AbstractTcpNetworkFactoryPtr &networkFactory) :
    m_pcbInterface(std::make_shared<Zera::cPCBInterface>())
{
    m_pcbInterface->setClientSuperSmart(networkInfo, networkFactory);
}

PcbServiceConnection::PcbServiceConnection(const ModuleNetworkParamsPtr &networkParams) :
    PcbServiceConnection(networkParams->m_pcbServiceConnectionInfo,
                         networkParams->m_tcpNetworkFactory)
{
}

TaskTemplatePtr PcbServiceConnection::createConnectionTask() const
{
    return TaskServerConnectionStart::create(m_pcbInterface->getClientSmart(), CONNECTION_TIMEOUT);
}

Zera::PcbInterfacePtr PcbServiceConnection::getInterface() const
{
    return m_pcbInterface;
}
