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
