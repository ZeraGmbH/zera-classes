#ifndef RMINITFORTEST_H
#define RMINITFORTEST_H

#include "proxyclientfortest.h"
#include "servertestanswers.h"
#include <rminterface.h>
#include <timerrunnerfortest.h>
#include <zeratimerfactorymethodstest.h>

class RmInitForTest
{
public:
    RmInitForTest();
    Zera::Server::RMInterfacePtr getRmInterface();
    Zera::Proxy::ProxyClientForTestPtr getProxyClient();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    Zera::Proxy::ProxyClientForTestPtr m_proxyClient;
};

#endif // RMINITFORTEST_H
