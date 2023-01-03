#ifndef PCBINITFORTEST_H
#define PCBINITFORTEST_H

#include "proxyclientfortest.h"
#include "servertestanswers.h"
#include <pcbinterface.h>
#include <timerrunnerfortest.h>
#include <zeratimerfactorymethodstest.h>

class PcbInitForTest
{
public:
    PcbInitForTest();
    Zera::Server::PcbInterfacePtr getPcbInterface();
    Zera::Proxy::ProxyClientForTestPtr getProxyClient();
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    Zera::Proxy::ProxyClientForTestPtr m_proxyClient;
};

#endif // PCBINITFORTEST_H
