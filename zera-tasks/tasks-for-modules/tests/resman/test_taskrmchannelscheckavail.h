#ifndef TEST_TASKRMCHANNELSCHECKAVAIL_H
#define TEST_TASKRMCHANNELSCHECKAVAIL_H

#include <rminterface.h>
#include "proxyclientfortest.h"

class test_taskrmchannelscheckavail : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void okOnExpectedEqualGet();
    void okOnExpectedPartOfGet();
    void errOnExpectedNotPartOfGet();
    void timeoutAndErrFunc();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKRMCHANNELSCHECKAVAIL_H
