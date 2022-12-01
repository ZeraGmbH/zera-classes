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
    TaskParallelPtr taskSeq = TaskParallel::create();
    int okCount = 0;
    int errCount = 0;
    int taskIdReceived = 42;
    connect(taskSeq.get(), &TaskParallel::sigFinish, [&](bool ok, int taskId) {
        taskIdReceived = taskId;
        if(ok)
            okCount++;
        else
            errCount++;
    });
    int taskId = taskSeq->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
    QCOMPARE(taskId, taskIdReceived);
}

void test_taskparallel::startErrorTask()
{
    TaskParallelPtr taskSeq = TaskParallel::create();
    taskSeq->addTask(TaskForTest::create(0, false));
    int okCount = 0;
    int errCount = 0;
    int taskIdReceived = 42;
    connect(taskSeq.get(), &TaskParallel::sigFinish, [&](bool ok, int taskId) {
        taskIdReceived = taskId;
        if(ok)
            okCount++;
        else
            errCount++;
    });
    int taskId = taskSeq->start();
    QCoreApplication::processEvents();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QCOMPARE(taskId, taskIdReceived);
}


