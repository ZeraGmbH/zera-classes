#include "test_tasksequence.h"
#include "tasksequence.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <timerrunnerfortest.h>
#include <QTest>

QTEST_MAIN(test_tasksequence)

void test_tasksequence::init()
{
    TaskForTestNew::resetCounters();
    TimerRunnerForTest::reset();
}

void test_tasksequence::startEmpty()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 0);
}

void test_tasksequence::oneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::twoOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(2*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 2);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 2);
    QCOMPARE(helper.signalDelayMs(), 2*DEFAULT_EXPIRE);
}

void test_tasksequence::oneError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(2*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::twoError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(2*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 2);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::threeError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(3*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 3);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::oneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(2*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 2);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::oneOkOneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, false));
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    TimerRunnerForTest::getInstance()->processTimers(3*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 1);
    QCOMPARE(TaskForTestNew::dtorCount(), 3);
    QCOMPARE(helper.signalDelayMs(), 2*DEFAULT_EXPIRE);
}

void test_tasksequence::taskId()
{
    TaskSequence task1;
    TaskTestHelperNew helper1(&task1);
    task1.addSub(TaskForTestNew::create(0, true));
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskSequence task2;
    TaskTestHelperNew helper2(&task2);
    task2.addSub(TaskForTestNew::create(0, true));
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

void test_tasksequence::startTwice()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    task->start();
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 0);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);

    TimerRunnerForTest::getInstance()->processTimers(DEFAULT_EXPIRE_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 1);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 1);
    QCOMPARE(helper.signalDelayMs(), DEFAULT_EXPIRE);
}

void test_tasksequence::onRunningAddAndStartOne()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();

    task->addSub(TaskForTestNew::create(DEFAULT_EXPIRE, true));
    task->start();
    QCOMPARE(TaskForTestNew::okCount(), 0);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 0);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);

    TimerRunnerForTest::getInstance()->processTimers(2*DEFAULT_EXPIRE);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 2);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 2);
    QCOMPARE(helper.signalDelayMs(), 2*DEFAULT_EXPIRE);
}

void test_tasksequence::twoTransactions()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelperNew helper(task.get());
    task->addSub(TaskForTestNew::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);

    task->addSub(TaskForTestNew::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 2);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTestNew::okCount(), 2);
    QCOMPARE(TaskForTestNew::errCount(), 0);
    QCOMPARE(TaskForTestNew::dtorCount(), 2);
}

