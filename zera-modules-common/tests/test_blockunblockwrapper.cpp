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
    QVERIFY(waiter.WaitForSignal(&timer, &QTimer::timeout,
                                 this, &test_blockunblockwrapper::dummyError));
}

void test_blockunblockwrapper::detectNoSignalOnAbort()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        waiter.abort();
    });
    QVERIFY(waiter.WaitForSignal(&timer, &QTimer::timeout,
                                 this, &test_blockunblockwrapper::dummyError));
}

void test_blockunblockwrapper::detectError()
{
    SignalWaiter waiter;
    QTimer timer;
    timer.start(1);
    connect(&timer, &QTimer::timeout, this, [&]() {
        emit dummyError("foo");
    });
    QVERIFY(waiter.WaitForSignal(&timer, &QTimer::timeout,
                                 this, &test_blockunblockwrapper::dummyError));
    QCOMPARE(waiter.getError(), "foo");
}
