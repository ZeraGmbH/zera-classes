#include "test_taskservertransactiontemplate.h"
#include "taskservertransactiontemplatetest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include "rminterface.h"
#include "rmtestanswers.h"
#include "proxyclientfortest.h"
#include <QTest>

QTEST_MAIN(test_taskservertransactiontemplate)

static const char* defaultResponse = "m0;m1;m2";

void test_taskservertransactiontemplate::actionOnAck()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, defaultResponse);
}

void test_taskservertransactiontemplate::noActionOnNack()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, "");
}

void test_taskservertransactiontemplate::okSignalOnAck()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errSignalOnNack()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(nack, defaultResponse);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskservertransactiontemplate::noReceiveOnOther()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::MSG_ID_OTHER);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errReceiveOnTcpError()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::TCP_ERROR);
    proxyClient->setAnswers(answers);

    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(rmInterface, received);

    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}


