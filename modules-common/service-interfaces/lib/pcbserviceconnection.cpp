#include "pcbserviceconnection.h"
#include "taskserverconnectionstart.h"
#include <proxy.h>

PcbServiceConnection::PcbServiceConnection(ModuleNetworkParamsPtr networkParams)
{
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(networkParams->m_pcbServiceConnectionInfo,
                                                                 networkParams->m_tcpNetworkFactory);
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbInterface->setClientSmart(m_pcbClient);
}

TaskTemplatePtr PcbServiceConnection::createConnectionTask() const
{
    return TaskServerConnectionStart::create(m_pcbClient, CONNECTION_TIMEOUT);
}

Zera::PcbInterfacePtr PcbServiceConnection::getInterface() const
{
    return m_pcbInterface;
}
