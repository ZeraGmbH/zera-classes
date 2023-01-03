#include "pcbinitfortest.h"

PcbInitForTest::PcbInitForTest()
{
    m_pcbInterface =  std::make_shared<Zera::Server::cPCBInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_pcbInterface->setClientSmart(m_proxyClient);
    TimerRunnerForTest::reset();
    ZeraTimerFactoryMethodsTest::enableTest();
}

Zera::Server::PcbInterfacePtr PcbInitForTest::getPcbInterface()
{
    return m_pcbInterface;
}

ProxyClientForTestPtr PcbInitForTest::getProxyClient()
{
    return m_proxyClient;
}

