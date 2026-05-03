#include "test_timelimitedtail.h"
#include "timelimitedtail.h"
#include <timemachinefortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_timelimitedtail)

void test_timelimitedtail::initialEmpty()
{
    TimeLimitedTail tail;
    QCOMPARE(tail.getValues().count(), 0);
}

void test_timelimitedtail::appendOneSizeOne()
{
    TimeLimitedTail tail;
    QSignalSpy spy(&tail, &TimeLimitedTail::sigContentsChanged);
    tail.appendValues(QVector<float>() << 1.0);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(tail.getValues().count(), 1);
    QCOMPARE(tail.getValues()[0][0], 1.0);
}

void test_timelimitedtail::appendTwoSizeTwo()
{
    TimeLimitedTail tail;
    QSignalSpy spy(&tail, &TimeLimitedTail::sigContentsChanged);
    tail.appendValues(QVector<float>() << 1.0 << 2.0);
    tail.appendValues(QVector<float>() << 3.0 << 4.0);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(tail.getValues().count(), 2);
    QCOMPARE(tail.getValues()[0][0], 1.0);
    QCOMPARE(tail.getValues()[0][1], 2.0);
    QCOMPARE(tail.getValues()[1][0], 3.0);
    QCOMPARE(tail.getValues()[1][1], 4.0);
}
