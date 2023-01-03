#include "test_taskrmcheckresourcetype.h"
#include "taskrmcheckresourcetype.h"
#include "rminitfortest.h"
#include "tasktesthelper.h"
#include "scpifullcmdcheckerfortest.h"
#include <QTest>

QTEST_MAIN(test_taskrmcheckresourcetype)

void test_taskrmcheckresourcetype::checkScpiSend()
{
    RmInitForTest rm;
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), EXPIRE_INFINITE);
    task->start();
    QCoreApplication::processEvents();
    QStringList scpiSent = rm.getProxyClient()->getReceivedCommands();
    QCOMPARE(scpiSent.count(), 1);
    ScpiFullCmdCheckerForTest scpiChecker("RESOURCE:TYPE:CAT", SCPI::isQuery);
    QVERIFY(scpiChecker.matches(scpiSent[0]));
}

void test_taskrmcheckresourcetype::okOnMatchingResourceLowerCase()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "sense:foo"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::okOnMatchingResourceUpperCase()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "SENSE:FOO"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskrmcheckresourcetype::errorOnNoResources()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, ""));
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmcheckresourcetype::errorOnMissingResource()
{
    RmInitForTest rm;
    rm.getProxyClient()->setAnswers(ServerTestAnswerList() << ServerTestAnswer(ack, "foo:bar"));
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), EXPIRE_INFINITE);
    TaskTestHelper helper(task.get());
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskrmcheckresourcetype::timeoutAndErrFunc()
{
    RmInitForTest rm;
    int localErrorCount = 0;
    TaskCompositePtr task = TaskRmCheckResourceType::create(rm.getRmInterface(), DEFAULT_EXPIRE,
                                                           [&]{
        localErrorCount++;
    });
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(localErrorCount, 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}
