#include "test_modulegroupnumerator.h"
#include "modulegroupnumerator.h"
#include <QTest>

QTEST_MAIN(test_modulegroupnumerator)

void test_modulegroupnumerator::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modulegroupnumerator::requestThreeZeroModuleNumbers()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(ModuleGroupNumerator::NoNumRequest), 1);
    QCOMPARE(numerator.requestModuleNum(ModuleGroupNumerator::NoNumRequest), 2);
    QCOMPARE(numerator.requestModuleNum(ModuleGroupNumerator::NoNumRequest), 3);
}

void test_modulegroupnumerator::requestOneFourModuleNumber()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(4), 4);
}

void test_modulegroupnumerator::requestOneTwoModuleNumberAndTwoZeroModuleNumber()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(2), 2);
    QCOMPARE(numerator.requestModuleNum(ModuleGroupNumerator::NoNumRequest), 1);
    QCOMPARE(numerator.requestModuleNum(ModuleGroupNumerator::NoNumRequest), 3);
}

void test_modulegroupnumerator::requestTwoOneModuleNumbers()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(1), 1);
    QCOMPARE(numerator.requestModuleNum(1), -1);
}
