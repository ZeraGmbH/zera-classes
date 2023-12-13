#include "test_modulegroupnumerator.h"
#include "modulegroupnumerator.h"
#include <QTest>

QTEST_MAIN(test_modulegroupnumerator)

void test_modulegroupnumerator::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modulegroupnumerator::test_add123()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    QCOMPARE(numerator.requestModuleNum(2), 2);
    QCOMPARE(numerator.requestModuleNum(3), 3);
}

void test_modulegroupnumerator::test_add11_error()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    QCOMPARE(numerator.requestModuleNum(1), -1);
}

void test_modulegroupnumerator::test_add1remove1add1()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    numerator.freeModuleNum(1);
    QCOMPARE(numerator.requestModuleNum(1), 1);
}

void test_modulegroupnumerator::test_add1remove2add1_error()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    numerator.freeModuleNum(2);
    QCOMPARE(numerator.requestModuleNum(1), -1);
}

void test_modulegroupnumerator::test_add12remove1add1()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    QCOMPARE(numerator.requestModuleNum(2), 2);
    numerator.freeModuleNum(1);
    QCOMPARE(numerator.requestModuleNum(1), 1);
}

