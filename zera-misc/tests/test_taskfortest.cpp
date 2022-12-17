#include "test_taskfortest.h"
#include "taskfortest.h"
#include "tasktesthelper.h"
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
    TaskTestHelper helper(&task);

    task.start();
    QCoreApplication::processEvents();

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QVERIFY(helper.signalDelay() < DELAY_TIME/2);
}

void test_taskfortest::oneErrImmediate()
{
    TaskForTest task(0, false);
    TaskTestHelper helper(&task);

    task.start();
    QCoreApplication::processEvents();

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QVERIFY(helper.signalDelay() < DELAY_TIME/2);
}

void test_taskfortest::onePassDelayed()
{
    TaskForTest task(DELAY_TIME, true);
    TaskTestHelper helper(&task);

    task.start();
    QTest::qWait(2*DELAY_TIME);

    QCOMPARE(helper.okCount(), 1);
    QCOMPARE(helper.errCount(), 0);
    QVERIFY(helper.signalDelay() >= DELAY_TIME);
}

void test_taskfortest::oneErrDelayed()
{
    TaskForTest task(DELAY_TIME, false);
    TaskTestHelper helper(&task);

    task.start();
    QTest::qWait(2*DELAY_TIME);

    QCOMPARE(helper.okCount(), 0);
    QCOMPARE(helper.errCount(), 1);
    QVERIFY(helper.signalDelay() >= DELAY_TIME);
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
    TaskTestHelper helper1(&task1);
    int taskId1 = task1.getTaskId();
    task1.start();
    QCOMPARE(helper1.lastTaskIdReceived(), taskId1);

    TaskForTest task2(0, true);
    TaskTestHelper helper2(&task2);
    int taskId2 = task2.getTaskId();
    task2.start();
    QCOMPARE(helper2.lastTaskIdReceived(), taskId2);
    QVERIFY(taskId1 != taskId2);
}

