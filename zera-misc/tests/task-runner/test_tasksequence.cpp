#include "test_tasksequence.h"
#include "tasksequence.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_tasksequence)

void test_tasksequence::init()
{
    TaskForTest::resetCounters();
}

void test_tasksequence::startEmpty()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
}

void test_tasksequence::oneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_tasksequence::twoOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_tasksequence::oneError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_tasksequence::twoError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_tasksequence::threeError()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_tasksequence::oneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_tasksequence::oneOkOneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_tasksequence::taskId()
{
    TaskSequence task1;
    TaskTestHelper helper1(&task1);
    task1.addSub(TaskForTest::create(0, true));
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskSequence task2;
    TaskTestHelper helper2(&task2);
    task2.addSub(TaskForTest::create(0, true));
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

void test_tasksequence::startTwice()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();
    task->start();
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_tasksequence::onRunningAddAndStartOne()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();

    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_tasksequence::twoTransactions()
{
    TaskContainerPtr task = TaskSequence::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);

    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(helper.okCount(), 2);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

