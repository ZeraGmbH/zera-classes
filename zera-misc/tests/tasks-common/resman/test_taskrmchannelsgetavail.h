#ifndef TEST_TASKRMCHANNELSAVAILGET_H
#define TEST_TASKRMCHANNELSAVAILGET_H

#include "rminterface.h"
#include "proxyclientfortest.h"

class test_taskrmchannelsgetavail : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void getThreeChannels();
    void getThreeChannelsIgnoreMMode();
    void timeoutAndErrFunc();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKRMCHANNELSAVAILGET_H
