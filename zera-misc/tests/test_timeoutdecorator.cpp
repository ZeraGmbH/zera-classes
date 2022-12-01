#include "test_timeoutdecorator.h"
#include "taskfortest.h"
#include "tasktimeoutdecorator.h"
#include <QTest>


QTEST_MAIN(test_timeoutdecorator)

static constexpr int DELAY_TIME = 10;

void test_timeoutdecorator::init()
{
    TaskForTest::resetCounters();
}

void test_timeoutdecorator::startEmpty()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(nullptr, 1000);
    int okCount = 0;
    int errCount = 0;
    int taskIdReceived = 42;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok, int taskId) {
        taskIdReceived = taskId;
        if(ok)
            okCount++;
        else
            errCount++;
    });
    int taskId = task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 0);
    QCOMPARE(taskId, taskIdReceived);
}

void test_timeoutdecorator::oneOkWithoutDelay()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(TaskForTest::create(0, true), 1000);
    int okCount = 0;
    int errCount = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
        delay = timer.elapsed();
    });
    timer.start();
    task->start();
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QVERIFY(delay < DELAY_TIME/2);
}

void test_timeoutdecorator::oneErrWithoutDelay()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(TaskForTest::create(0, false), 1000);
    int okCount = 0;
    int errCount = 0;
    int delay = 1000;
    QElapsedTimer timer;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
        delay = timer.elapsed();
    });
    timer.start();
    task->start();
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QVERIFY(delay < DELAY_TIME/2);

}

void test_timeoutdecorator::oneOkWithDelayAndInfiniteTimeout()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(TaskForTest::create(1000, true), DELAY_TIME);
    int okCount = 0;
    int errCount = 0;
    int delay = 0;
    QElapsedTimer timer;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
        delay = timer.elapsed();
    });
    timer.start();
    task->start();
    QTest::qWait(1.5*DELAY_TIME);
    QCOMPARE(okCount, 0);
    QCOMPARE(errCount, 1);
    QCOMPARE(TaskForTest::getOkCount(), 0);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QVERIFY(delay >= DELAY_TIME);
    QVERIFY(delay < 2*DELAY_TIME);
}

void test_timeoutdecorator::noTimeoutOnEarlyOk()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(TaskForTest::create(DELAY_TIME/2, true), DELAY_TIME);
    int okCount = 0;
    int errCount = 0;
    int delay = 0;
    QElapsedTimer timer;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok) {
        if(ok)
            okCount++;
        else
            errCount++;
        delay = timer.elapsed();
    });
    timer.start();
    task->start();
    QTest::qWait(1.5*DELAY_TIME);
    QCOMPARE(okCount, 1);
    QCOMPARE(errCount, 0);
    QCOMPARE(TaskForTest::getOkCount(), 1);
    QCOMPARE(TaskForTest::getErrCount(), 0);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QVERIFY(delay >= DELAY_TIME/2);
    QVERIFY(delay < 2*DELAY_TIME);
}

void test_timeoutdecorator::delayEqualsTimeout()
{
    TaskTimeoutDecoratorPtr task = TaskTimeoutDecorator::create(TaskForTest::create(DELAY_TIME, true), DELAY_TIME);
    int okCount = 0;
    int errCount = 0;
    int delay = 0;
    int taskIdReceived = 42;
    QElapsedTimer timer;
    connect(task.get(), &TaskTimeoutDecorator::sigFinish, [&](bool ok, int taskId) {
        taskIdReceived = taskId;
        if(ok)
            okCount++;
        else
            errCount++;
        delay = timer.elapsed();
    });
    timer.start();
    int taskId = task->start();
    QTest::qWait(1.5*DELAY_TIME);
    QCOMPARE(okCount+errCount, 1);
    QCOMPARE(TaskForTest::getDtorCount(), 1);
    QVERIFY(delay >= DELAY_TIME);
    QVERIFY(delay < 2*DELAY_TIME);
    QCOMPARE(taskIdReceived, taskId);
}

