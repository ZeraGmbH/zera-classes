#include "test_taskfortest.h"
#include "taskfortest.h"
#include <QTest>
#include <QElapsedTimer>

QTEST_MAIN(test_taskfortest)

static constexpr int DELAY_TIME = 10;

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
