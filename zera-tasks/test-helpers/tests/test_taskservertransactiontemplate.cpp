#include "test_taskservertransactiontemplate.h"
#include "taskservertransactiontemplatetest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include "rmtestanswers.h"
#include <QTest>

QTEST_MAIN(test_taskservertransactiontemplate)

static const char* defaultResponse = "m0;m1;m2";

void test_taskservertransactiontemplate::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskservertransactiontemplate::actionOnAck()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, defaultResponse);
}

void test_taskservertransactiontemplate::noActionOnNack()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(nack, defaultResponse));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(received, "");
}

void test_taskservertransactiontemplate::okSignalOnAck()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errSignalOnNack()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(nack, defaultResponse));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskservertransactiontemplate::noReceiveOnOther()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::MSG_ID_OTHER));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskservertransactiontemplate::errReceiveOnTcpError()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, defaultResponse, RmTestAnswer::TCP_ERROR));
    QString received;
    TaskCompositePtr task = TaskServerTransactionTemplateTest::create(m_rmInterface, received);
    TaskTestHelperNew helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
