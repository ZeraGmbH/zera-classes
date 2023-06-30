#include "rminitfortest.h"
#include <timemachinefortest.h>

RmInitForTest::RmInitForTest()
{
    m_rmInterface =  std::make_shared<Zera::cRMInterface>();
    m_proxyClient = Zera::ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
    TimeMachineForTest::reset();
    TimerFactoryQtForTest::enableTest();
}

Zera::RMInterfacePtr RmInitForTest::getRmInterface()
{
    return m_rmInterface;
}

Zera::ProxyClientForTestPtr RmInitForTest::getProxyClient()
{
    return m_proxyClient;
}
