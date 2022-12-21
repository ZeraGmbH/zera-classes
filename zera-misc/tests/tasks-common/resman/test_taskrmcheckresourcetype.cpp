#include "test_taskrmcheckresourcetype.h"
#include "taskrmcheckresourcetype.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskrmcheckresourcetype)

void test_taskrmcheckresourcetype::init()
{
    m_rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    m_proxyClient = ProxyClientForTest::create();
    m_rmInterface->setClientSmart(m_proxyClient);
}

void test_taskrmcheckresourcetype::okOnMatchingResourceLowerCase()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, "sense:foo"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(m_rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::okOnMatchingResourceUpperCase()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, "SENSE:FOO"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(m_rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::errorOnNoResources()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, ""));
    TaskCompositePtr task = TaskRmCheckResourceType::create(m_rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmcheckresourcetype::errorOnMissingResource()
{
    m_proxyClient->setAnswers(RmTestAnswerList() << RmTestAnswer(ack, "foo:bar"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(m_rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
