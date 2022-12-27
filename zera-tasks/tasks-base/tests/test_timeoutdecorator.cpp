#include "test_timeoutdecorator.h"
#include "taskfortest.h"
#include "tasktimeoutdecorator.h"
#include "tasktesthelper.h"
#include <singleshottimertest.h>
#include <timerrunnerfortest.h>
#include <QTest>

QTEST_MAIN(test_timeoutdecorator)

void test_timeoutdecorator::init()
{
    TaskForTest::resetCounters();
    TimerRunnerForTest::reset();
}

static TaskCompositePtr wrapSimulatedTimeout(int timeout, TaskCompositePtr decoratedTask)
{
    return std::make_unique<TaskTimeoutDecorator>(
                std::make_unique<SingleShotTimerTest>(timeout),
                std::move(decoratedTask));
}

void test_timeoutdecorator::startEmpty()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(EXPIRE_INFINITE, nullptr);
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
    TaskCompositePtr task = wrapSimulatedTimeout(1, nullptr);
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
}

void test_timeoutdecorator::immediateOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(EXPIRE_INFINITE, TaskForTest::create(0, true));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_timeoutdecorator::immediateError()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(EXPIRE_INFINITE, TaskForTest::create(0, false));
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_timeoutdecorator::infiniteTask()
{
    TaskCompositePtr task = wrapSimulatedTimeout(DEFAULT_EXPIRE, TaskForTest::create(EXPIRE_INFINITE, true));
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_timeoutdecorator::delayedOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_EXPIRE, TaskForTest::create(DEFAULT_EXPIRE/2, true));
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE/2);
}

void test_timeoutdecorator::delayEqualsTimeout()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_EXPIRE, TaskForTest::create(DEFAULT_EXPIRE, true));
    TaskTestHelper helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount() + helper.errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
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

