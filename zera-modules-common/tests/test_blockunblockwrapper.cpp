#include "test_blockunblockwrapper.h"
#include "signalwaiter.h"
#include <QTest>
#include <QTimer>

QTEST_MAIN(test_blockunblockwrapper)

void test_blockunblockwrapper::detectSignal()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.WaitForSignal(&timer, &QTimer::timeout));
}

void test_blockunblockwrapper::detectSignalWithNoneErrSig()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_OK_SIG, waiter.WaitForSignal(&timer, &QTimer::timeout,
                                                             this, &test_blockunblockwrapper::sigNone));
}

void test_blockunblockwrapper::handleAbort()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        waiter.abort();
    });
    QCOMPARE(SignalWaiter::WAIT_ABORT, waiter.WaitForSignal(this, &test_blockunblockwrapper::sigNone));
}

void test_blockunblockwrapper::detectError()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    QCOMPARE(SignalWaiter::WAIT_ERR_SIG, waiter.WaitForSignal(this, &test_blockunblockwrapper::sigNone,
                                                              &timer, &QTimer::timeout));
}

void test_blockunblockwrapper::detectTimeout()
{
    SignalWaiter waiter;
    QCOMPARE(SignalWaiter::WAIT_TIMEOUT, waiter.WaitForSignal(this, &test_blockunblockwrapper::sigNone,
                                                              this, &test_blockunblockwrapper::sigNone, 1));
}
