#include "test_tasksequence.h"
#include "tasksequence.h"
#include "taskfortest.h"
#include <QElapsedTimer>
#include <QTest>

QTEST_MAIN(test_tasksequence)

void test_tasksequence::init()
{
    TaskForTest::resetCounters();
}

void test_tasksequence::startEmpty()
{
    TaskContainerPtr task = TaskSequence::create();
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

void test_tasksequence::oneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_tasksequence::twoOk()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, true));
    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::oneError()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_tasksequence::twoError()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, false));
    task->addSubTask(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::threeError()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, false));
    task->addSubTask(TaskForTest::create(0, false));
    task->addSubTask(TaskForTest::create(0, false));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_tasksequence::oneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, false));
    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::oneOkOneErrorOneOk()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, true));
    task->addSubTask(TaskForTest::create(0, false));
    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_tasksequence::taskId()
{
    TaskSequence task1;
    task1.addSubTask(TaskForTest::create(0, true));
    int taskIdReceived1 = 42;
    connect(&task1, &TaskSequence::sigFinish, [&](bool, int taskId) {
        taskIdReceived1 = taskId;
    } );
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(taskIdReceived1, taskId1);

    TaskSequence task2;
    task2.addSubTask(TaskForTest::create(0, true));
    int taskIdReceived2 = 67;
    connect(&task2, &TaskSequence::sigFinish, [&](bool, int taskId) {
        taskIdReceived2 = taskId;
    } );
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(taskIdReceived2, taskId2);
    QVERIFY(taskId1 != taskId2);
}

void test_tasksequence::startTwice()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(DELAY_TIME, true));
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

void test_tasksequence::onRunningAddAndStartOne()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(DELAY_TIME, true));
    task->start();

    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 0);

    QTest::qWait(DELAY_TIME*1.5);

    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::twoTransactions()
{
    TaskContainerPtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskContainer::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);

    task->addSubTask(TaskForTest::create(0, true));
    task->start();
    QCOMPARE(okCount, 2);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

