#ifndef RMINITFORTEST_H
#define RMINITFORTEST_H

#include "proxyclientfortest.h"
#include "rmtestanswers.h"
#include <rminterface.h>
#include <tasktesthelper.h>
#include <scpifullcmdcheckerfortest.h>
#include <timerrunnerfortest.h>
#include <zeratimerfactorymethodstest.h>

class RmInitForTest
{
public:
    RmInitForTest();
    Zera::Server::RMInterfacePtr getRmInterface();
    ProxyClientForTestPtr getProxyClient();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // RMINITFORTEST_H
