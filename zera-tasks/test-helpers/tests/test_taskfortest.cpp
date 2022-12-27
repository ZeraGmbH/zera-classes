#include "test_taskfortest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <timerrunnerfortest.h>
#include <QTest>

QTEST_MAIN(test_taskfortest)

void test_taskfortest::init()
{
    TaskForTestNew::resetCounters();
    TimerRunnerForTest::reset();
}

void test_taskfortest::onePassImmediateNew()
{
    TaskForTestNew task(0, true);
    TaskTestHelperNew helper(&task);

    task.start();

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
}

void test_taskfortest::oneErrImmediateNew()
{
    TaskForTestNew task(0, false);
    TaskTestHelperNew helper(&task);

    task.start();

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
}

void test_taskfortest::onePassDelayedNew()
{
    TaskForTestNew task(DEFAULT_EXPIRE, true);
    TaskTestHelperNew helper(&task);

    task.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskfortest::oneErrDelayedNew()
{
    TaskForTestNew task(DEFAULT_EXPIRE, false);
    TaskTestHelperNew helper(&task);

    task.start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_taskfortest::onePassCountNew()
{
    TaskForTestNew task(0, true);
    task.start();
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 0);
}

void test_taskfortest::oneErrCountNew()
{
    TaskForTestNew task(0, false);
    task.start();
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
}

void test_taskfortest::oneDtorCountNew()
{
    {
        TaskForTestNew task(0, true);
    }
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
}

void test_taskfortest::taskIdNew()
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

