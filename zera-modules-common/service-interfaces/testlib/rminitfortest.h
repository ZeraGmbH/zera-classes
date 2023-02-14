#ifndef RMINITFORTEST_H
#define RMINITFORTEST_H

#include "proxyclientfortest.h"
#include "servertestanswers.h"
#include <rminterface.h>
#include <timerfactoryqtfortest.h>

class RmInitForTest
{
public:
    RmInitForTest();
    Zera::RMInterfacePtr getRmInterface();
    Zera::ProxyClientForTestPtr getProxyClient();
private:
    Zera::RMInterfacePtr m_rmInterface;
    Zera::ProxyClientForTestPtr m_proxyClient;
};

#endif // RMINITFORTEST_H
