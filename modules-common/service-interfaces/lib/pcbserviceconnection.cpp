#include "pcbserviceconnection.h"
#include <proxy.h>

void PcbServiceConnection::openConnection(ModuleNetworkParamsPtr networkParams)
{
    m_pcbInterface = std::make_shared<Zera::cPCBInterface>();
    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart(networkParams->m_pcbServiceConnectionInfo,
                                                                                  networkParams->m_tcpNetworkFactory);
    m_pcbInterface->setClientSmart(m_pcbClient);
}

Zera::PcbInterfacePtr PcbServiceConnection::getInterface()
{
    return m_pcbInterface;
}

Zera::ProxyClientPtr PcbServiceConnection::getClient()
{
    return m_pcbClient;
}
