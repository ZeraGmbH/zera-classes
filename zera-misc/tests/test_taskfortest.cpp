#include "test_taskfortest.h"
#include "taskfortest.h"
#include <QTest>
#include <QElapsedTimer>

QTEST_MAIN(test_taskfortest)

void test_taskfortest::init()
{
    TaskForTest::resetCounters();
}

void test_taskfortest::onePassImmediate()
{
    TaskForTest task(0, true);
    int countOk = 0;
    int countErr = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::sigFinish, [&](bool ok) {
        if(ok)
            countOk++;
        else
            countErr++;
        delay = timer.elapsed();
    } );

    timer.start();
    task.start();
    QCoreApplication::processEvents();

    QCOMPARE(countOk, 1);
    QCOMPARE(countErr, 0);
    QVERIFY(delay < DELAY_TIME/2);
}

void test_taskfortest::oneErrImmediate()
{
    TaskForTest task(0, false);
    int countOk = 0;
    int countErr = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::sigFinish, [&](bool ok) {
        if(ok)
            countOk++;
        else
            countErr++;
        delay = timer.elapsed();
    });

    timer.start();
    task.start();
    QCoreApplication::processEvents();

    QCOMPARE(countOk, 0);
    QCOMPARE(countErr, 1);
    QVERIFY(delay < DELAY_TIME/2);
}

void test_taskfortest::onePassDelayed()
{
    TaskForTest task(DELAY_TIME, true);
    int countOk = 0;
    int countErr = 0;
    int delay = 0;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::sigFinish, [&](bool ok) {
        if(ok)
            countOk++;
        else
            countErr++;
        delay = timer.elapsed();
    });

    timer.start();
    task.start();
    QTest::qWait(2*DELAY_TIME);

    QCOMPARE(countOk, 1);
    QCOMPARE(countErr, 0);
    QVERIFY(delay >= DELAY_TIME);
}

void test_taskfortest::oneErrDelayed()
{
    TaskForTest task(DELAY_TIME, false);
    int countOk = 0;
    int delay = 0;
    int countErr = 0;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::sigFinish, [&](bool ok) {
        if(ok)
            countOk++;
        else
            countErr++;
        delay = timer.elapsed();
    } );

    timer.start();
    task.start();
    QTest::qWait(2*DELAY_TIME);

    QCOMPARE(countOk, 0);
    QCOMPARE(countErr, 1);
    QVERIFY(delay >= DELAY_TIME);
}

void test_taskfortest::onePassCount()
{
    TaskForTest task(0, true);
    task.start();
    QCoreApplication::processEvents();
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
}

void test_taskfortest::oneErrCount()
{
    TaskForTest task(0, false);
    task.start();
    QCoreApplication::processEvents();
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
}

void test_taskfortest::oneDtorCount()
{
    {
        TaskForTest task(0, true);
    }
    QCOMPARE(TaskForTest::getDtorCount(), 1);
}

void test_taskfortest::taskId()
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

