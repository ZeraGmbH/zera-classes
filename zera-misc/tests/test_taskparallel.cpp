#include "test_taskparallel.h"
#include "taskparallel.h"
#include "taskfortest.h"
#include <QTest>

QTEST_MAIN(test_taskparallel)

static constexpr int DELAY_TIME = 10;

void test_taskparallel::init()
{
    TaskForTest::resetCounters();
}

void test_taskparallel::startEmpty()
{
    TaskParallelPtr task = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
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
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(0, false));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_taskparallel::startPassImmediateDelayed()
{
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(0, true));
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();

    QCoreApplication::processEvents();
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
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(0, true));
    task->addTask(TaskForTest::create(0, false));
    task->addTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();

    QCoreApplication::processEvents();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::startThreeImmediateAllOk()
{
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(0, true));
    task->addTask(TaskForTest::create(0, true));
    task->addTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();

    QCoreApplication::processEvents();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 3);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_taskparallel::startThreeDelayedMiddleFail()
{
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    task->addTask(TaskForTest::create(DELAY_TIME, false));
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
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
    TaskParallelPtr task = TaskParallel::create();
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    task->addTask(TaskForTest::create(DELAY_TIME, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskParallel::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
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
    TaskForTest task1(0, true);
    int taskIdReceived1 = 42;
    connect(&task1, &TaskForTest::sigFinish, [&](bool, int taskId) {
        taskIdReceived1 = taskId;
    } );
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(taskIdReceived1, taskId1);

    TaskForTest task2(0, true);
    int taskIdReceived2 = 67;
    connect(&task2, &TaskForTest::sigFinish, [&](bool, int taskId) {
        taskIdReceived2 = taskId;
    } );
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(taskIdReceived2, taskId2);
    QVERIFY(taskId1 != taskId2);
}


