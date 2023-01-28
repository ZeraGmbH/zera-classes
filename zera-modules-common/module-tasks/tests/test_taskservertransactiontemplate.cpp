#include "test_taskservertransactiontemplate.h"
#include "taskservertransactiontemplatetest.h"
#include "taskfortest.h"
#include "servertestanswers.h"
#include <timemachinefortest.h>
#include <tasktesthelper.h>
#include <QTest>

QTEST_MAIN(test_taskservertransactiontemplate)

static const char* defaultResponse = "m0;m1;m2";

void test_taskservertransactiontemplate::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = Zera::Proxy::ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskservertransactiontemplate::actionOnAck()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, defaultResponse);
}

void test_taskservertransactiontemplate::noActionOnNack()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(nack, defaultResponse));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, "");
}

void test_taskservertransactiontemplate::okSignalOnAck()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errSignalOnNack()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(nack, defaultResponse));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskservertransactiontemplate::noReceiveOnOther()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse, ServerTestAnswer::MSG_ID_OTHER));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errReceiveOnTcpError()
{
    m_proxyClient->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, defaultResponse, ServerTestAnswer::TCP_ERROR));
    QString received;
    TaskTemplatePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
