#include "test_modulegroupnumerator.h"
#include "modulegroupnumerator.h"
#include <QTest>

QTEST_MAIN(test_modulegroupnumerator)

void test_modulegroupnumerator::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_modulegroupnumerator::requestThreeZeros()
{
    ModuleGroupNumerator numerator;
    QCOMPARE(numerator.requestModuleNum(0), 1);
    QCOMPARE(numerator.requestModuleNum(0), 2);
    QCOMPARE(numerator.requestModuleNum(0), 3);
}
