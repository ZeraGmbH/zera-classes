#include "test_timeoutdecorator.h"
#include "taskfortest.h"
#include "tasktimeoutdecorator.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_timeoutdecorator)

void test_timeoutdecorator::init()
{
    TaskForTest::resetCounters();
}

void test_timeoutdecorator::startEmpty()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(1000, nullptr);
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
}

void test_timeoutdecorator::startEmptyCheckDelayed()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(1, nullptr);
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
}

void test_timeoutdecorator::oneOkWithoutDelay()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(1000, TaskForTest::create(0, true));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT/2);
}

void test_timeoutdecorator::oneErrWithoutDelay()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(1000, TaskForTest::create(0, false));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT/2);

}

void test_timeoutdecorator::oneOkWithDelayAndInfiniteTimeout()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(1000, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(1.5*DEFAULT_TIMEOUT);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT);
    QVERIFY(helper.signalDelayMs() < 2*DEFAULT_TIMEOUT);
}

void test_timeoutdecorator::noTimeoutOnEarlyOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(DEFAULT_TIMEOUT/2, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(1.5*DEFAULT_TIMEOUT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT/2);
    QVERIFY(helper.signalDelayMs() < 2*DEFAULT_TIMEOUT);
}

void test_timeoutdecorator::delayEqualsTimeout()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(DEFAULT_TIMEOUT, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(1.5*DEFAULT_TIMEOUT);
    QCOMPARE(helper.okCount() + helper.errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT);
    QVERIFY(helper.signalDelayMs() < 2*DEFAULT_TIMEOUT);
}

void test_timeoutdecorator::taskId()
{
    TaskForTest task1(0, true);
    TaskTestHelper helper1(&task1);
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskForTest task2(0, true);
    TaskTestHelper helper2(&task2);
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

