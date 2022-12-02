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
    TaskSequencePtr task = TaskSequence::create();
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
}

void test_tasksequence::oneOk()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_tasksequence::twoOk()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, true));
    task->appendTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 2);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::oneError()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, false));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
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

void test_tasksequence::twoError()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, false));
    task->appendTask(TaskForTest::create(0, false));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
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
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::threeError()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, false));
    task->appendTask(TaskForTest::create(0, false));
    task->appendTask(TaskForTest::create(0, false));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
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
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_tasksequence::oneErrorOneOk()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, false));
    task->appendTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
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
    QCOMPARE(TaskForTest::getDtorCount(), 2);
}

void test_tasksequence::oneOkOneErrorOneOk()
{
    TaskSequencePtr task = TaskSequence::create();
    task->appendTask(TaskForTest::create(0, true));
    task->appendTask(TaskForTest::create(0, false));
    task->appendTask(TaskForTest::create(0, true));
    int okCount = 0;
    int errCount = 0;
    connect(task.get(), &TaskSequence::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
    });
    task->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 3);
}

void test_tasksequence::taskId()
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

