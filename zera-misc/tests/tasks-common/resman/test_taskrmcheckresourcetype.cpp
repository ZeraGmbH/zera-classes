#include "test_taskrmcheckresourcetype.h"
#include "taskrmcheckresourcetype.h"
#include "rminterface.h"
#include "rmtestanswers.h"
#include "tasktesthelper.h"
#include "proxyclientfortest.h"
#include <QTest>

QTEST_MAIN(test_taskrmcheckresourcetype)

void test_taskrmcheckresourcetype::okOnMatchingResourceLowerCase()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "sense:foo");
    proxyClient->setAnswers(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::okOnMatchingResourceUpperCase()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "SENSE:FOO");
    proxyClient->setAnswers(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::errorOnNoResources()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "");
    proxyClient->setAnswers(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmcheckresourcetype::errorOnMissingResource()
{
    Zera::Server::RMInterfacePtr rmInterface =  std::make_shared<Zera::Server::cRMInterface>();
    ProxyClientForTestPtr proxyClient = ProxyClientForTest::create();
    rmInterface->setClientSmart(proxyClient);

    QList<RmTestAnswer> answers = QList<RmTestAnswer>() << RmTestAnswer(ack, "foo:bar");
    proxyClient->setAnswers(answers);

    TaskCompositePtr task = TaskRmCheckResourceType::create(rmInterface, DELAY_TIME);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}
