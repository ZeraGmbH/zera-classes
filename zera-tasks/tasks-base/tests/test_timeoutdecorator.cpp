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
    TaskForTestNew::resetCounters();
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
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 0);
}

void test_timeoutdecorator::startEmptyCheckDelayed()
{
    TaskCompositePtr task = wrapSimulatedTimeout(1, nullptr);
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 0);
}

void test_timeoutdecorator::immediateOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(EXPIRE_INFINITE, TaskForTestNew::create(0, true));
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
}

void test_timeoutdecorator::immediateError()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(EXPIRE_INFINITE, TaskForTestNew::create(0, false));
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
}

void test_timeoutdecorator::infiniteTask()
{
    TaskCompositePtr task = wrapSimulatedTimeout(DEFAULT_EXPIRE, TaskForTestNew::create(EXPIRE_INFINITE, true));
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_timeoutdecorator::delayedOk()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_EXPIRE, TaskForTestNew::create(DEFAULT_EXPIRE/2, true));
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE/2);
}

void test_timeoutdecorator::delayEqualsTimeout()
{
    TaskCompositePtr task = TaskTimeoutDecorator::wrapTimeout(DEFAULT_EXPIRE, TaskForTestNew::create(DEFAULT_EXPIRE, true));
    TaskTestHelperNew helper(task.get());
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount() + helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_timeoutdecorator::taskId()
{
    TaskForTestNew task1(0, true);
    TaskTestHelperNew helper1(&task1);
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskForTestNew task2(0, true);
    TaskTestHelperNew helper2(&task2);
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

