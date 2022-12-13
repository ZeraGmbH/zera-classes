#include "test_taskparallel.h"
#include "taskparallel.h"
#include "taskfortest.h"
#include <QTest>

QTEST_MAIN(test_taskparallel)

void test_taskparallel::init()
{
    TaskForTest::resetCounters();
}

void test_taskparallel::startEmpty()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
}

void test_taskparallel::startErrorTask()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_taskparallel::startPassImmediateDelayed()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->start();

    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);

    QTest::qWait(DELAY_TIME*1.5);
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_taskparallel::startThreeImmediateMiddleFail()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, false));
    task->addSub(TaskForTest::create(0, true));
    task->start();

    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::startThreeImmediateAllOk()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, true));
    task->addSub(TaskForTest::create(0, true));
    task->start();

    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 3);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::startThreeDelayedMiddleFail()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->addSub(TaskForTest::create(DELAY_TIME, false));
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->start();

    QTest::qWait(DELAY_TIME*1.5);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::startThreeDelayedAllOk()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->start();

    QTest::qWait(DELAY_TIME*1.5);
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 3);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::taskId()
{
    TaskParallel task1;
    task1.addSub(TaskForTest::create(0, true));
    int taskIdReceived1 = 42;
    connect(&task1, &TaskParallel::sigFinish, [&](bool, int taskId) {
        taskIdReceived1 = taskId;
    } );
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(taskIdReceived1, taskId1);

    TaskParallel task2;
    task2.addSub(TaskForTest::create(0, true));
    int taskIdReceived2 = 67;
    connect(&task2, &TaskParallel::sigFinish, [&](bool, int taskId) {
        taskIdReceived2 = taskId;
    } );
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(taskIdReceived2, taskId2);
    QVERIFY(taskId1 != taskId2);
}

void test_taskparallel::startTwice()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->start();
    task->start();
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 0);

    QTest::qWait(DELAY_TIME*1.5);

    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_taskparallel::onRunningAddAndStartOne()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(DELAY_TIME, true));
    task->start();

    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 0);

    QTest::qWait(DELAY_TIME*1.5);

    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_taskparallel::twoTransactions()
{
    TaskContainerPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);

    task->addSub(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 2);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}


