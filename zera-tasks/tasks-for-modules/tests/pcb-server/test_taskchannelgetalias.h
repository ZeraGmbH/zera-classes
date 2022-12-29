#ifndef TEST_TASKCHANNELGETALIAS_H
#define TEST_TASKCHANNELGETALIAS_H

#include "proxyclientfortest.h"
#include <pcbinterface.h>

class test_taskchannelgetalias : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void checkScpiSend();
    void returnsAliasProperly();
    void timeoutAndErrFunc();
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKCHANNELGETALIAS_H
