#include "rminitfortest.h"

RmInitForTest::RmInitForTest()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = Zera::Proxy::ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
    TimerRunnerForTest::reset();
    ZeraTimerFactoryMethodsTest::enableTest();
}

Zera::Server::RMInterfacePtr RmInitForTest::getRmInterface()
{
    return m_rmInterface;
}

Zera::Proxy::ProxyClientForTestPtr RmInitForTest::getProxyClient()
{
    return m_proxyClient;
}
