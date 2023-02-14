#ifndef PCBINITFORTEST_H
#define PCBINITFORTEST_H

#include "proxyclientfortest.h"
#include "servertestanswers.h"
#include <pcbinterface.h>
#include <timerfactoryqtfortest.h>

class PcbInitForTest
{
public:
    PcbInitForTest();
    Zera::PcbInterfacePtr getPcbInterface();
    Zera::ProxyClientForTestPtr getProxyClient();
private:
    Zera::PcbInterfacePtr m_pcbInterface;
    Zera::ProxyClientForTestPtr m_proxyClient;
};

#endif // PCBINITFORTEST_H
