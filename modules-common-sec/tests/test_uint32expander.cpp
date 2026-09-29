#include "test_uint32expander.h"
#include "uint32bitexpander.h"
#include <QTest>

QTEST_MAIN(test_uint32expander)

void test_uint32expander::initZero()
{
    UInt32BitExpander expander;

    QCOMPARE(expander.getActualExpanded64(), 0);
    QCOMPARE(expander.getActualExpandedDbl(), 0.0);
}

void test_uint32expander::setValueOnce()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getActualExpandedDbl(), 42.0);
}

void test_uint32expander::setValueTwiceWithoutOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(1);
    expander.setActual32(2);
    QCOMPARE(expander.getActualExpanded64(), 2);
    QCOMPARE(expander.getActualExpandedDbl(), 2.0);
}

void test_uint32expander::setValueTwiceWithoutOverflowEqual()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(42);
    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getActualExpandedDbl(), 42.0);
}

void test_uint32expander::setValueTwiceWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(3);

    const quint64 expected = (1ULL << 32) + 3;
    QCOMPARE(expander.getActualExpanded64(), expected);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expected));
}

void test_uint32expander::setValueMultipleOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(41);
    expander.setActual32(40);
    expander.setActual32(39);

    const quint64 expected = 3*(1ULL << 32) + 39;
    QCOMPARE(expander.getActualExpanded64(), expected);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expected));
}

void test_uint32expander::setValueTwiceWithOverflowThenReset()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(3);

    const quint64 expected = (1ULL << 32) + 3;
    QCOMPARE(expander.getActualExpanded64(), expected);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expected));

    expander.reset();
    QCOMPARE(expander.getActualExpanded64(), 0);
    QCOMPARE(expander.getActualExpandedDbl(), 0.0);
}

void test_uint32expander::doubleLimits()
{
    QCOMPARE(static_cast<double>(1ULL << 32), 4294967296.0);
    QCOMPARE(static_cast<double>(1ULL << 53), 9007199254740992.0);

    const quint64 im2 = (1ULL << 53)-2;
    const quint64 im1 = im2+1;
    const quint64 i0 = im2+2;
    const quint64 i1 = im2 + 3;

    const double dm2 = static_cast<double>(im2);
    const double dm1 = static_cast<double>(im1);
    const double d0 = static_cast<double>(i0);
    const double d1 = static_cast<double>(i1);

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
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(42);

    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getActualExpandedDbl(), 42.0);
    QCOMPARE(expander.getFinalExpanded64(), 42);
    QCOMPARE(expander.getFinalExpandedDbl(), 42.0);
}

void test_uint32expander::actEqualFinalWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setActual32(42);
    expander.setFinal32(42);

    const quint64 expected = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expected);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expected));
    QCOMPARE(expander.getFinalExpanded64(), expected);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expected));
}

void test_uint32expander::actLessThanFinalNoOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(40);
    expander.setFinal32(42);

    QCOMPARE(expander.getActualExpanded64(), 40);
    QCOMPARE(expander.getActualExpandedDbl(), 40.0);
    QCOMPARE(expander.getFinalExpanded64(), 42);
    QCOMPARE(expander.getFinalExpandedDbl(), 42.0);
}

void test_uint32expander::actLessThanFinalWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(5000);
    expander.setActual32(40);
    expander.setFinal32(42);

    const quint64 expectedActual = (1ULL << 32) + 40;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 42;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::actLargerThanFinalNoOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(40);

    QCOMPARE(expander.getActualExpanded64(), 42);
    QCOMPARE(expander.getActualExpandedDbl(), 42.0);
    QCOMPARE(expander.getFinalExpanded64(), 40);
    QCOMPARE(expander.getFinalExpandedDbl(), 40.0);
}

void test_uint32expander::actLargerThanFinalWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(5000);
    expander.setActual32(42);
    expander.setFinal32(40);

    const quint64 expectedActual = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 40;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::actLessThanOverflowFinal()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(0);

    const quint64 expectedActual = (1ULL << 32) - 1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::actLargerThanOverflowFinal()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setActual32(0);
    expander.setFinal32((1ULL << 32)-1);

    const quint64 expectedActual = (1ULL << 32);
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActOnlyNoOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(42);

    const quint64 expectedActual = 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = 0;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalActNoOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32(42);
    expander.setFinal32(43);
    expander.setActual32(44);

    const quint64 expectedActual = 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setFinalActFinalNoOverflow()
{
    UInt32BitExpander expander;

    expander.setFinal32(42);
    expander.setActual32(43);
    expander.setFinal32(44);

    const quint64 expectedActual = 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActOnlyWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setActual32(42);

    const quint64 expectedActual = (1ULL << 32) + 42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalWithOverflow()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(43);

    const quint64 expectedActual = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setFinalActWithOverflow()
{
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(43);

    const quint64 expectedActual = (1ULL << 32)+43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalActWithOverflow1()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32(43);
    expander.setActual32(44);

    const quint64 expectedActual = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalActWithOverflow2()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(44);

    const quint64 expectedActual = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setFinalActFinalwithOverflow()
{
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    expander.setActual32(43);
    expander.setFinal32(44);

    const quint64 expectedActual = (1ULL << 32) + 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActualSmallerFinalOverflowActual()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    expander.setFinal32((1ULL << 32)-2);
    expander.setActual32(2);

    const quint64 expectedActual = (1ULL << 32) + 2;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) - 2;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActOnlyWithOverflowImmediateRead()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual1));

    expander.setActual32(42);
    const quint64 expectedActual2 = (1ULL << 32)+42;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual2));

    const quint64 expectedFinal = (1ULL << 32);
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));

    QCOMPARE(expander.getActualExpanded64(), expectedActual2);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual2));
}

void test_uint32expander::setActFinalWithOverflowImmediateRead()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual = (1ULL << 32)-1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    expander.setFinal32(43);
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setFinalActWithOverflowImmediateRead()
{
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32)-1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));

    expander.setActual32(43);
    const quint64 expectedActual = (1ULL << 32)+43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual));

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalActWithOverflow1ImmediateRead()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32) - 1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual1));

    expander.setFinal32(43);
    const quint64 expectedFinal = (1ULL << 32) + 43;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));

    expander.setActual32(44);
    const quint64 expectedActual2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual2));

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setActFinalActWithOverflow2ImmediateRead()
{
    UInt32BitExpander expander;

    expander.setActual32((1ULL << 32)-1);
    const quint64 expectedActual1 = (1ULL << 32) -1;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual1));

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));

    expander.setActual32(44);
    const quint64 expectedActual2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getActualExpanded64(), expectedActual2);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual2));

    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));
}

void test_uint32expander::setFinalActFinalwithOverflowImmediateRead()
{
    UInt32BitExpander expander;

    expander.setFinal32((1ULL << 32)-1);
    const quint64 expectedFinal = (1ULL << 32) - 1;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal));

    expander.setActual32(43);
    const quint64 expectedActual1 = (1ULL << 32) + 43;
    QCOMPARE(expander.getActualExpanded64(), expectedActual1);
    QCOMPARE(expander.getActualExpandedDbl(), static_cast<double>(expectedActual1));

    expander.setFinal32(44);
    const quint64 expectedFinal2 = (1ULL << 32) + 44;
    QCOMPARE(expander.getFinalExpanded64(), expectedFinal2);
    QCOMPARE(expander.getFinalExpandedDbl(), static_cast<double>(expectedFinal2));
}
