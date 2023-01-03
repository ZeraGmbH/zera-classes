#ifndef PCBINITFORTEST_H
#define PCBINITFORTEST_H

#include "proxyclientfortest.h"
#include "rmtestanswers.h"
#include <pcbinterface.h>
#include <tasktesthelper.h>
#include <scpifullcmdcheckerfortest.h>
#include <timerrunnerfortest.h>
#include <zeratimerfactorymethodstest.h>

class PcbInitForTest
{
public:
    PcbInitForTest();
    Zera::Server::PcbInterfacePtr getPcbInterface();
    ProxyClientForTestPtr getProxyClient();
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // PCBINITFORTEST_H
