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
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(TIMEOUT_INFINITE, nullptr);
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

void test_timeoutdecorator::immediateOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(TIMEOUT_INFINITE, TaskForTest::create(0, true));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT_MIN);
}

void test_timeoutdecorator::immediateError()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(TIMEOUT_INFINITE, TaskForTest::create(0, false));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT_MIN);

}

void test_timeoutdecorator::infiniteTask()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(TIMEOUT_INFINITE, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT_WAIT);
}

void test_timeoutdecorator::delayedOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(DEFAULT_TIMEOUT_MIN, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT_MIN);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT_WAIT);
}

void test_timeoutdecorator::delayEqualsTimeout()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_TIMEOUT, TaskForTest::create(DEFAULT_TIMEOUT, true));
    TaskTestHelper helper(task.get());
    task->start();
    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount() + helper.errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QVERIFY(helper.signalDelayMs() >= DEFAULT_TIMEOUT);
    QVERIFY(helper.signalDelayMs() < DEFAULT_TIMEOUT_WAIT);
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

