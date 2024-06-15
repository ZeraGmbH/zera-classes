#include "test_totalmemorytracker.h"
#include "totalmemorytracker.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_totalmemorytracker)

void test_totalmemorytracker::test_initial_zero()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfoAllZero");
    memorytracker.calculateMemoryUsedPercent();
    QCOMPARE(memorytracker.getMemoryUsageParams().m_RAMUsedPercent, 0.0);
    QCOMPARE(memorytracker.getMemoryUsageParams().m_buffersUsedPercent, 0.0);
    QCOMPARE(memorytracker.getMemoryUsageParams().m_cachesUsedPercent, 0.0);
}

void test_totalmemorytracker::testMemoryUsed()
{
    TotalMemoryTracker memorytracker;
    TestSystemInfoFileLocator::setProcMeminfoFileName(":/procMeminfo");
    memorytracker.calculateMemoryUsedPercent();
    QCOMPARE(memorytracker.getMemoryUsageParams().m_RAMUsedPercent, 25.0);
    QCOMPARE(memorytracker.getMemoryUsageParams().m_buffersUsedPercent, 15.0);
    QCOMPARE(memorytracker.getMemoryUsageParams().m_cachesUsedPercent, 20.0);
}
