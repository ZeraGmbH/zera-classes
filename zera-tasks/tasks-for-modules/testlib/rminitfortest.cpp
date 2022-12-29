#include "rminitfortest.h"

RmInitForTest::RmInitForTest()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
    TimerRunnerForTest::reset();
    ZeraTimerFactoryMethodsTest::enableTest();
}

Zera::Server::RMInterfacePtr RmInitForTest::getRmInterface()
{
    return m_rmInterface;
}

ProxyClientForTestPtr RmInitForTest::getProxyClient()
{
    return m_proxyClient;
}
