#ifndef TEST_TASKRMSENDIDENT_H
#define TEST_TASKRMSENDIDENT_H

#include "rminterface.h"
#include "proxyclientfortest.h"

class test_taskrmsendident : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void checkSend();
    void timeoutAndErrFunc();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKRMSENDIDENT_H
