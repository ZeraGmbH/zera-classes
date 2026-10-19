#include "test_uint32expander.h"
#include "uint32bitexpander.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_uint32expander)

void test_uint32expander::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_uint32expander::init()
{
    TimeMachineForTest::reset();
}

void test_uint32expander::initZero()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    QCOMPARE(expander.getActualExpanded64(), 0);
}

void test_uint32expander::setValueOnce()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), 42);
}

void test_uint32expander::setValueTwiceWithoutOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(1);
    expander.setActual32(2);
    QCOMPARE(expander.getActualExpanded64(), 2);
}

void test_uint32expander::setValueTwiceWithoutOverflowEqual()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), 42);
}

void test_uint32expander::setValueTwiceWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(3);

    const quint64 expected = (1ULL << 32) + 3;
    QCOMPARE(expander.getActualExpanded64(), expected);
}

void test_uint32expander::setValueMultipleOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(41);
    expander.setActual32(40);
    expander.setActual32(39);

    const quint64 expected = 3*(1ULL << 32) + 39;
    QCOMPARE(expander.getActualExpanded64(), expected);
}

void test_uint32expander::setValueTwiceWithOverflowThenReset()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(3);

    const quint64 expected = (1ULL << 32) + 3;
    QCOMPARE(expander.getActualExpanded64(), expected);

    expander.resetForNextPeriod();
    QCOMPARE(expander.getActualExpanded64(), 0);
}

void test_uint32expander::doubleLimits()
{
    QCOMPARE(UInt32BitExpander::uint64ToDbl(1ULL << 32), 4294967296.0);
    QCOMPARE(UInt32BitExpander::uint64ToDbl(1ULL << 53), 9007199254740992.0);

    const quint64 im2 = (1ULL << 53)-2;
    const quint64 im1 = im2+1;
    const quint64 i0 = im2+2;
    const quint64 i1 = im2 + 3;

    const double dm2 = UInt32BitExpander::uint64ToDbl(im2);
    const double dm1 = UInt32BitExpander::uint64ToDbl(im1);
    const double d0 = UInt32BitExpander::uint64ToDbl(i0);
    const double d1 = UInt32BitExpander::uint64ToDbl(i1);

    QCOMPARE(dm2, 9007199254740990.0);
    QCOMPARE(dm1, 9007199254740991.0);
    QCOMPARE(d0 , 9007199254740992.0);

    // rounding (might fail due to arch/compiler)
    // see https://stackoverflow.com/questions/2044124/what-happens-in-c-when-an-integer-type-is-cast-to-a-floating-point-type-or-vic
    // 'For reference, this is what ISO-IEC 14882-2003 says' .. second part
    // For our use case:
    // * reaching values 2⁵³ is highly unlikely
    // * even if: inacurracy caused by rounding is way below what we can measure
    QCOMPARE(d1 , 9007199254740992.0);
}

void test_uint32expander::actEqualFinalNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(42);

    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getFinalExpanded64(), 42);
}

void test_uint32expander::actEqualFinalWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setActual32(42);
    expander.setFinal32(42);

    const quint64 expected = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expected);
    QCOMPARE(expander.getFinalExpanded64(), expected);
}

void test_uint32expander::actLessThanFinalNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(40);
    expander.setFinal32(42);

    QCOMPARE(expander.getActualExpanded64(), 40);
    QCOMPARE(expander.getFinalExpanded64(), 42);
}

void test_uint32expander::actLessThanFinalWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(5000);
    expander.setActual32(40);
    expander.setFinal32(42);

    const quint64 expectedActual = (1ULL << 32) + 40;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 42;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::actLargerThanFinalNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(40);

    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getFinalExpanded64(), 40);
}

void test_uint32expander::actLargerThanFinalWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(5000);
    expander.setActual32(42);
    expander.setFinal32(40);

    const quint64 expectedActual = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 40;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::actLessThanOverflowFinal()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(0);

    const quint64 expectedActual = (1ULL << 32) - 1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::actLargerThanOverflowFinal()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(0);
    expander.setFinal32((1ULL << 32)-1);

    const quint64 expectedActual = (1ULL << 32);
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActOnlyNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);

    const quint64 expectedActual = 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = 0;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalActNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(43);
    expander.setActual32(44);

    const quint64 expectedActual = 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setFinalActFinalNoOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setFinal32(42);
    expander.setActual32(43);
    expander.setFinal32(44);

    const quint64 expectedActual = 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActOnlyWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setActual32(42);

    const quint64 expectedActual = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(43);

    const quint64 expectedActual = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setFinalActWithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(43);

    const quint64 expectedActual = (1ULL << 32)+43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalActWithOverflow1()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(43);
    expander.setActual32(44);

    const quint64 expectedActual = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalActWithOverflow2()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(44);

    const quint64 expectedActual = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setFinalActFinalwithOverflow()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(43);
    expander.setFinal32(44);

    const quint64 expectedActual = (1ULL << 32) + 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActualSmallerFinalOverflowActual()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32((1ULL << 32)-2);
    expander.setActual32(2);

    const quint64 expectedActual = (1ULL << 32) + 2;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) - 2;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActOnlyWithOverflowImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);

    expander.setActual32(42);
    const quint64 expectedActual2 = (1ULL << 32)+42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);

    QCOMPARE(expander.getActualExpanded64(), expectedActual2);
}

void test_uint32expander::setActFinalWithOverflowImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    expander.setFinal32(43);
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setFinalActWithOverflowImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);

    expander.setActual32(43);
    const quint64 expectedActual = (1ULL << 32)+43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalActWithOverflow1ImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32) - 1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);

    expander.setFinal32(43);
    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);

    expander.setActual32(44);
    const quint64 expectedActual2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setActFinalActWithOverflow2ImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32) -1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);

    expander.setActual32(44);
    const quint64 expectedActual2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
}

void test_uint32expander::setFinalActFinalwithOverflowImmediateRead()
{
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);

    expander.setActual32(43);
    const quint64 expectedActual1 = (1ULL << 32) + 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);

    expander.setFinal32(44);
    const quint64 expectedFinal2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal2);
}

void test_uint32expander::actualHighBeforeFinalLowAfterSafeDelay()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(42);
    QCOMPARE(expander.getActualExpanded64(), 1000);
    QCOMPARE(expander.getFinalExpanded64(), 42);
}

void test_uint32expander::actualHighBeforeFinalLowAfterSafeDelayTwoSequences()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(42);
    QCOMPARE(expander.getActualExpanded64(), 1000);
    QCOMPARE(expander.getFinalExpanded64(), 42);

    expander.resetForNextPeriod();

    expander.setActual32(1001);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(43);
    QCOMPARE(expander.getActualExpanded64(), 1001);
    QCOMPARE(expander.getFinalExpanded64(), 43);
}

void test_uint32expander::actualHighLowBeforeFinalLowAfterSafeDelay()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    expander.setActual32(41);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(42);
    QCOMPARE(expander.getActualExpanded64(), 41);
    QCOMPARE(expander.getFinalExpanded64(), 42);
}

void test_uint32expander::actualHighLowBeforeFinalLowAfterSafeDelayTwoSequences()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    expander.setActual32(41);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(42);
    QCOMPARE(expander.getActualExpanded64(), 41);
    QCOMPARE(expander.getFinalExpanded64(), 42);

    expander.resetForNextPeriod();

    expander.setActual32(1001);
    expander.setActual32(44);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);

    expander.setFinal32(43);
    QCOMPARE(expander.getActualExpanded64(), 44);
    QCOMPARE(expander.getFinalExpanded64(), 43);
}

void test_uint32expander::actualHighBeforeActualHighAfterActualLowAfterSafeDelay()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    expander.setActual32((1ULL << 32)-1);

    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), (1ULL << 32) + 42);
    QCOMPARE(expander.getFinalExpanded64(), (1ULL << 32));
}

void test_uint32expander::actualHighBeforeActualHighAfterActualLowAfterSafeDelayTwoSequences()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    expander.setActual32((1ULL << 32)-1);

    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), (1ULL << 32) + 42);
    QCOMPARE(expander.getFinalExpanded64(), (1ULL << 32));

    expander.resetForNextPeriod();

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    expander.setActual32((1ULL << 32)-1);

    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), (1ULL << 32) + 42);
    QCOMPARE(expander.getFinalExpanded64(), (1ULL << 32));
}

void test_uint32expander::actualHighBeforeActualHighAfterFinalLowAfterSafeDelay()
{
    UInt32BitExpander expander;

    expander.setActual32(1000);
    TimeMachineForTest::getInstance()->processTimers(UInt32BitExpander::SafeDelayBeforFirstWraparound);
    expander.setActual32((1ULL << 32)-1);

    expander.setFinal32(42);
    QCOMPARE(expander.getActualExpanded64(), (1ULL << 32) -1);
    QCOMPARE(expander.getFinalExpanded64(), (1ULL << 32) + 42);
}

