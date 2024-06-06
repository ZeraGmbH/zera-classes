#include "test_cpuload.h"
#include "cpuload.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_cpuload)

void test_cpuload::test_initial_zero()
{
    CpuLoad cpuLoad;
    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuAllZero");
    cpuLoad.calcNextValues();
    QMap<QString, float> loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 0.0);
    QCOMPARE(loads["1"], 0.0);
}

void test_cpuload::test_user_load()
{
    CpuLoad cpuLoad;
    cpuLoad.setWarningLimit(0.49);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad");
    cpuLoad.calcNextValues(); // warning in log expected
    QMap<QString, float> loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 50.0);
    QCOMPARE(loads["1"], 25.0);
}

void test_cpuload::test_nice_load()
{
    CpuLoad cpuLoad;
    cpuLoad.setWarningLimit(0.5);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuNiceLoad");
    cpuLoad.calcNextValues(); // no warning in log expected
    QMap<QString, float> loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 50.0);
    QCOMPARE(loads["1"], 25.0);
}

void test_cpuload::test_user_load_multiple()
{
    CpuLoad cpuLoad;
    cpuLoad.setWarningLimit(0.49);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad");
    cpuLoad.calcNextValues(); // warning in log expected
    QMap<QString, float> loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 50.0);
    QCOMPARE(loads["1"], 25.0);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad1");
    cpuLoad.calcNextValues(); // no warning in log expected
    loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 50.0);
    QCOMPARE(loads["1"], 25.0);

    TestSystemInfoFileLocator::setProcStatusFileName(":/procStatOneCpuUserLoad2");
    cpuLoad.calcNextValues(); // info in log expected
    loads = cpuLoad.getLoadMapForDisplay();
    QCOMPARE(loads["All"], 25.0);
    QCOMPARE(loads["1"], 100.0);
}
