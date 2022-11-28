#include "test_taskfortest.h"
#include "taskfortest.h"
#include <QTest>
#include <QElapsedTimer>

QTEST_MAIN(test_taskfortest)

static constexpr int DELAY_TIME = 10;

void test_taskfortest::onePassImmediate()
{
    TaskForTest task(0, false);
    int countOk = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::finishOk, [&]() {
        countOk++;
        delay = timer.elapsed();
    } );
    int countErr = 0;
    connect(&task, &TaskForTest::finishErr, [&]() {
        countErr++;
    });

    timer.start();
    task.start();
    QCoreApplication::processEvents();

    QCOMPARE(countOk, 1);
    QCOMPARE(countErr, 0);
    QVERIFY(delay < DELAY_TIME/2);
}

void test_taskfortest::oneErrImmediate()
{
    TaskForTest task(0, true);
    int countOk = 0;
    connect(&task, &TaskForTest::finishOk, [&]() {
        countOk++;
    });
    int countErr = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::finishErr, [&]() {
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
    TaskForTest task(DELAY_TIME, false);
    int countOk = 0;
    int delay = 0;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::finishOk, [&]() {
        countOk++;
        delay = timer.elapsed();
    } );
    int countErr = 0;
    connect(&task, &TaskForTest::finishErr, [&]() {
        countErr++;
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
    TaskForTest task(DELAY_TIME, true);
    int countOk = 0;
    int delay = 0;
    QElapsedTimer timer;
    connect(&task, &TaskForTest::finishOk, [&]() {
        countOk++;
    } );
    int countErr = 0;
    connect(&task, &TaskForTest::finishErr, [&]() {
        countErr++;
        delay = timer.elapsed();
    });

    timer.start();
    task.start();
    QTest::qWait(2*DELAY_TIME);

    QCOMPARE(countOk, 0);
    QCOMPARE(countErr, 1);
    QVERIFY(delay >= DELAY_TIME);
}

