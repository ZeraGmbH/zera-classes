#include "test_totalmemorytracker.h"
#include "totalmemorytracker.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_totalmemorytracker)

void test_totalmemorytracker::test_initial_zero()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfoAllZero");
    float memoryUsed = memorytracker.CalculateMemoryUsedPercent();
    QCOMPARE(memoryUsed, 0.0);
}

void test_totalmemorytracker::testMemoryUsed()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfo");
    float memoryUsed = memorytracker.CalculateMemoryUsedPercent();
    QCOMPARE(memoryUsed, 75.0);
}


