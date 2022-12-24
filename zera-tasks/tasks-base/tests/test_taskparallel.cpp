#include "test_taskparallel.h"
#include "taskparallel.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
#include <QTest>

QTEST_MAIN(test_taskparallel)

void test_taskparallel::init()
{
    TaskForTest::resetCounters();
}

void test_taskparallel::startEmpty()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->start();
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 0);
}

void test_taskparallel::startErrorTask()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 0);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 1);
}

void test_taskparallel::startPassImmediateDelayed()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_taskparallel::startThreeImmediateMiddleFail()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, true));
    task->start();

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_taskparallel::startThreeImmediateAllOk()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, true));
    task->start();

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 3);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_taskparallel::startThreeDelayedMiddleFail()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, false));
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 1);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_taskparallel::startThreeDelayedAllOk()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);
    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 3);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 3);
}

void test_taskparallel::taskId()
{
    TaskParallel task1;
    TaskTestHelper helper1(&task1);
    task1.addSub(TaskForTest::create(0, true));
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskParallel task2;
    TaskTestHelper helper2(&task2);
    task2.addSub(TaskForTest::create(0, true));
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

void test_taskparallel::startTwice()
{
    TaskContainerPtr task = TaskParallel::create();
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

void test_taskparallel::onRunningAddAndStartOne()
{
    TaskContainerPtr task = TaskParallel::create();
    TaskTestHelper helper(task.get());
    task->addSub(TaskForTest::create(DEFAULT_TIMEOUT, true));
    task->start();

    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(TaskForTest::okCount(), 1);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 1);
    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 0);

    QTest::qWait(DEFAULT_TIMEOUT_WAIT);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QCOMPARE(TaskForTest::okCount(), 2);
    QCOMPARE(TaskForTest::errCount(), 0);
    QCOMPARE(TaskForTest::dtorCount(), 2);
}

void test_taskparallel::twoTransactions()
{
    TaskContainerPtr task = TaskParallel::create();
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


