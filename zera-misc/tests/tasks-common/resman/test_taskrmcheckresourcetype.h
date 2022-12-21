#ifndef TEST_TASKRMCHECKRESOURCETYPE_H
#define TEST_TASKRMCHECKRESOURCETYPE_H

#include "rminterface.h"
#include "proxyclientfortest.h"

class test_taskrmcheckresourcetype : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void okOnMatchingResourceLowerCase();
    void okOnMatchingResourceUpperCase();
    void errorOnNoResources();
    void errorOnMissingResource();
    void timeoutAndErrFunc();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKRMCHECKRESOURCETYPE_H
