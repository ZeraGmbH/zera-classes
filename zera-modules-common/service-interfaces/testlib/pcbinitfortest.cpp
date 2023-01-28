#include "pcbinitfortest.h"
#include <timemachinefortest.h>

PcbInitForTest::PcbInitForTest()
{
    m_pcbInterface =  std::make_shared<Zera::Server::cPCBInterface>();
    m_proxyClient = Zera::Proxy::ProxyClientForTest::create();
    m_pcbInterface->setClientSmart(m_proxyClient);
    TimeMachineForTest::reset();
    TimerFactoryQtForTest::enableTest();
}

Zera::Server::PcbInterfacePtr PcbInitForTest::getPcbInterface()
{
    return m_pcbInterface;
}

Zera::Proxy::ProxyClientForTestPtr PcbInitForTest::getProxyClient()
{
    return m_proxyClient;
}

