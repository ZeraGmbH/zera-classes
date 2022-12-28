#include "test_taskfortest.h"
#include "taskfortest.h"
#include <tasktesthelper.h>
#include <timerrunnerfortest.h>
#include <QTest>

QTEST_MAIN(test_taskfortest)

void test_taskfortest::init()
{
    TaskForTest::resetCounters();
    TimerRunnerForTest::reset();
}

void test_taskfortest::onePassImmediate()
{
    TaskForTest task(0, true);
    TaskTestHelper helper(&task);

    task.start();

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskfortest::oneErrImmediate()
{
    TaskForTest task(0, false);
    TaskTestHelper helper(&task);

    task.start();

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskfortest::onePassDelayed()
{
    TaskForTest task(DEFAULT_EXPIRE, true);
    TaskTestHelper helper(&task);

    task.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskfortest::oneErrDelayed()
{
    TaskForTest task(DEFAULT_EXPIRE, false);
    TaskTestHelper helper(&task);

    task.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskfortest::onePassCount()
{
    TaskForTest task(0, true);
    task.start();
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
}

void test_taskfortest::oneErrCount()
{
    TaskForTest task(0, false);
    task.start();
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
}

void test_taskfortest::oneDtorCount()
{
    {
        TaskForTest task(0, true);
    }
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_taskfortest::taskId()
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

