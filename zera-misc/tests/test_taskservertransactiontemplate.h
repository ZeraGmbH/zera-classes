#ifndef TEST_TASKSERVERTRANSACTIONTEMPLATE_H
#define TEST_TASKSERVERTRANSACTIONTEMPLATE_H

#include "rminterface.h"
#include "proxyclientfortest.h"

class test_taskservertransactiontemplate : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void actionOnAck();
    void noActionOnNack();
    void okSignalOnAck();
    void errSignalOnNack();
    void noReceiveOnOther();
    void errReceiveOnTcpError();
private:
    Zera::Server::RMInterfacePtr m_rmInterface;
    ProxyClientForTestPtr m_proxyClient;
};

#endif // TEST_TASKSERVERTRANSACTIONTEMPLATE_H
