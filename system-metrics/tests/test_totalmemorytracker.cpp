#include "test_totalmemorytracker.h"
#include "totalmemorytracker.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_totalmemorytracker)

void test_totalmemorytracker::test_initial_zero()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfoAllZero");
    memorytracker.CalculateMemoryUsedPercent();
    QCOMPARE(memorytracker.getMemoryUsedPercent(), 0.0);
}

void test_totalmemorytracker::testMemoryUsed()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfo");
    memorytracker.CalculateMemoryUsedPercent();
    QCOMPARE(memorytracker.getMemoryUsedPercent(), 75.0);
}


