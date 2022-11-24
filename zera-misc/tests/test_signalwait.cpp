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
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.wait());
}

void test_signalwait::detectDirectEarlyEmitSignal()
{
    SignalWaiter waiter(this, &test_signalwait::sigTest);
    emit sigTest();
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.wait());
}

void test_signalwait::detectSignalWithNoneErrSig()
{
    QTimer timer;
    SignalWaiter waiter(&timer, &QTimer::timeout,
                        this, &test_signalwait::sigNone);
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.wait());
}

void test_signalwait::handleAbort()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_signalwait::sigNone);
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        waiter.abort();
    });
    QCOMPARE(SignalWaiter::WAIT_ABORT, waiter.wait());
}

void test_signalwait::detectError()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_signalwait::sigNone,
                        &timer, &QTimer::timeout);
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_ERR_SIG, waiter.wait());
}

void test_signalwait::detectTimeout()
{
    SignalWaiter waiter(this, &test_signalwait::sigNone,
                        this, &test_signalwait::sigNone, 1);
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.wait());
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
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.wait());
    int elapsed = timer.elapsed();
    QVERIFY(elapsed >= 25);
}
