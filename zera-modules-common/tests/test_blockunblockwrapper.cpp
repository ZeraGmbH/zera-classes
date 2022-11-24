#include "test_blockunblockwrapper.h"
#include "signalwaiter.h"
#include <QTest>
#include <QTimer>

QTEST_MAIN(test_blockunblockwrapper)

void test_blockunblockwrapper::detectSignal()
{
    QTimer timer;
    SignalWaiter waiter(&timer, &QTimer::timeout);
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.wait());
}

void test_blockunblockwrapper::detectSignalWithNoneErrSig()
{
    QTimer timer;
    SignalWaiter waiter(&timer, &QTimer::timeout,
                        this, &test_blockunblockwrapper::sigNone);
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.wait());
}

void test_blockunblockwrapper::handleAbort()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_blockunblockwrapper::sigNone);
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        waiter.abort();
    });
    QCOMPARE(SignalWaiter::WAIT_ABORT, waiter.wait());
}

void test_blockunblockwrapper::detectError()
{
    QTimer timer;
    SignalWaiter waiter(this, &test_blockunblockwrapper::sigNone,
                        &timer, &QTimer::timeout);
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_ERR_SIG, waiter.wait());
}

void test_blockunblockwrapper::detectTimeout()
{
    SignalWaiter waiter(this, &test_blockunblockwrapper::sigNone,
                        this, &test_blockunblockwrapper::sigNone, 1);
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.wait());
}
