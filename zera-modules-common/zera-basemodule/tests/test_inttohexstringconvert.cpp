#include "test_inttohexstringconvert.h"
#include "inttohexstringconvert.h"
#include <QTest>

QTEST_MAIN(test_inttohexstringconvert)

void test_inttohexstringconvert::digitCount4()
{
    QCOMPARE(IntToHexStringConvert::convert(10, 4, false).size(), 4);
}

void test_inttohexstringconvert::digitCount6()
{
    QCOMPARE(IntToHexStringConvert::convert(10, 6, false).size(), 6);
}

void test_inttohexstringconvert::leadingZeros0Val()
{
    QCOMPARE(IntToHexStringConvert::convert(0, 4, false), "0000");
}

void test_inttohexstringconvert::heading0x()
{
    QCOMPARE(IntToHexStringConvert::convert(0, 4, true), "0x0000");
}

void test_inttohexstringconvert::valOneByte()
{
    QCOMPARE(IntToHexStringConvert::convert(0x12, 4, true), "0x0012");
}

void test_inttohexstringconvert::upperChars()
{
    QCOMPARE(IntToHexStringConvert::convert(0xABCD, 4, true), "0xABCD");
}

void test_inttohexstringconvert::moreDigitsThanMaxDigits()
{
    QCOMPARE(IntToHexStringConvert::convert(0x1ABCD, 4, true), "0x1ABCD");
}
