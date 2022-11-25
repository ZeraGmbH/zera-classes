#include "test_signalwait.h"
#include "signalwaiter.h"
#include <QTest>
#include <QTimer>

QTEST_MAIN(test_signalwait)

void test_signalwait::detectSignal()
{
    QTimer timer;
    SignalWaiter waiter(&timer, &QTimer::timeout);
    timer.start(1);
    QVERIFY(waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.getResult());
}

void test_signalwait::detectDirectEarlyEmitSignal()
{
    SignalWaiter waiter(this, &test_signalwait::sigTest);
    emit sigTest();
    QVERIFY(waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.getResult());
}

void test_signalwait::detectSignalWithNoneErrSig()
{
    QTimer timer;
    SignalWaiter waiter(&timer, &QTimer::timeout,
                        this, &test_signalwait::sigNone);
    timer.start(1);
    QVERIFY(waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.getResult());
}

void test_signalwait::handleAbort()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_signalwait::sigNone);
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        waiter.abort();
    });
    QVERIFY(!waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_ABORT, waiter.getResult());
}

void test_signalwait::detectError()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_signalwait::sigNone,
                        &timer, &QTimer::timeout);
    timer.start(1);
    QVERIFY(!waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_ERR_SIG, waiter.getResult());
}

void test_signalwait::detectTimeout()
{
    SignalWaiter waiter(this, &test_signalwait::sigNone,
                        this, &test_signalwait::sigNone, 1);
    QVERIFY(!waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.getResult());
}

void test_signalwait::timeoutStartsOnWait()
{
    SignalWaiter waiter(this, &test_signalwait::sigTest, 30);
    QEventLoop eventLoop;
    QTimer::singleShot(5, this, [&]() {
        eventLoop.quit();
    });
    eventLoop.exec();

    QElapsedTimer timer;
    timer.start();
    QVERIFY(!waiter.wait());
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.getResult());
    int elapsed = timer.elapsed();
    QVERIFY(elapsed >= 25);
}

void test_signalwait::dummyWaiterReturnsImmediately()
{
    SignalWaiter waiter;
    QElapsedTimer timer;
    timer.start();
    waiter.wait();
    int elapsed = timer.elapsed();
    QVERIFY(elapsed < 1);
    QCOMPARE(SignalWaiter::WAIT_UNDEF, waiter.getResult());
}
