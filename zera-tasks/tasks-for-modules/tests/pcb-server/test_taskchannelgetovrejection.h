#ifndef TEST_TASKCHANNELGETOVREJECTION_H
#define TEST_TASKCHANNELGETOVREJECTION_H

#include "proxyclientfortest.h"
#include <pcbinterface.h>

class test_taskchannelgetovrejection : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void checkScpiSend();
    void returnsOvrRejectionProperly();
    void timeoutAndErrFunc();
private:
    Zera::Server::PcbInterfacePtr m_pcbInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKCHANNELGETOVREJECTION_H
