#include "test_procstatdecoder.h"
#include "procstatdecoder.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

static constexpr int TestCpuCount = 32;

QTEST_MAIN(test_procstatdecoder)

void test_procstatdecoder::initTestCase()
{
    TestSystemInfoFileLocator::setProcStatusFileName(":/procStat");
}

void test_procstatdecoder::decodeProcStatLine0()
{
    QString procLine = "cpu  341778 175612 123963 58336335 59354 19710 10003 0 0 0";
    CpuTimingValues cpuData;
    int cpuNum = ProcStatDecoder::decodeCpuProcLine(procLine, cpuData);

    QCOMPARE(cpuNum, 0);
    QCOMPARE(cpuData.m_userTime, 341778);
    QCOMPARE(cpuData.m_niceTime, 175612);
    QCOMPARE(cpuData.m_systemTime, 123963);
    QCOMPARE(cpuData.m_idleTime, 58336335);
    QCOMPARE(cpuData.m_iowaitTime, 59354);
    QCOMPARE(cpuData.m_irqTime, 19710);
    QCOMPARE(cpuData.m_softIrqTime, 10003);
}

void test_procstatdecoder::decodeProcStatLine1()
{
    QString procLine = "cpu0 25887 5525 5450 1803831 494 714 3735 0 0 0";
    CpuTimingValues cpuData;
    int cpuNum = ProcStatDecoder::decodeCpuProcLine(procLine, cpuData);

    QCOMPARE(cpuNum, 1);
    QCOMPARE(cpuData.m_userTime, 25887);
    QCOMPARE(cpuData.m_niceTime, 5525);
    QCOMPARE(cpuData.m_systemTime, 5450);
    QCOMPARE(cpuData.m_idleTime, 1803831);
    QCOMPARE(cpuData.m_iowaitTime, 494);
    QCOMPARE(cpuData.m_irqTime, 714);
    QCOMPARE(cpuData.m_softIrqTime, 3735);
}

void test_procstatdecoder::decodeProcStat()
{
    QMap<int, CpuTimingValues> timings = ProcStatDecoder::getCpuTimings();
    QCOMPARE(timings.size(), TestCpuCount+1);

    QCOMPARE(timings[0].m_userTime, 341778);
    QCOMPARE(timings[0].m_niceTime, 175612);
    QCOMPARE(timings[0].m_systemTime, 123963);
    QCOMPARE(timings[0].m_idleTime, 58336335);
    QCOMPARE(timings[0].m_iowaitTime, 59354);
    QCOMPARE(timings[0].m_irqTime, 19710);
    QCOMPARE(timings[0].m_softIrqTime, 10003);

    QCOMPARE(timings[1].m_userTime, 25887);
    QCOMPARE(timings[1].m_niceTime, 5525);
    QCOMPARE(timings[1].m_systemTime, 5450);
    QCOMPARE(timings[1].m_idleTime, 1803831);
    QCOMPARE(timings[1].m_iowaitTime, 494);
    QCOMPARE(timings[1].m_irqTime, 714);
    QCOMPARE(timings[1].m_softIrqTime, 3735);
}

