#include "test_range_hardware_overload.h"
#include "testrangehelpers.h"
#include "demodspvaluesrange.h"
#include "testsniffer.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <controllerpersitentdata.h>
#include <controllerdefinitions.h>
#include <QTest>

QTEST_MAIN(test_range_hardware_overload)

void test_range_hardware_overload::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

static int constexpr rangeEntityId = 1020;
static const char* RangeAutomaticComponent("PAR_RangeAutomatic");
static const char* RangeGroupingComponent("PAR_ChannelGrouping");

static constexpr int DspChannelUl1 = 0; // m0
static constexpr int DspChannelIl1 = 1; // m3
static constexpr int DspChannelUl2 = 2; // m1
static constexpr int DspChannelIl2 = 3; // m4

static const char* UL1RangeComponent("PAR_Channel1Range");
static const char* IL1RangeComponent("PAR_Channel4Range");

static constexpr int channelCountInpected = 8;

void test_range_hardware_overload::init()
{
    TimeMachineForTest::reset();
    // to avoid surprises: run as COM5003 - that should report overload
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-range-test.json", /*initialAdjPermission=*/true, "com5003");
    m_sampleDspInterfaceProg = m_testRunner->findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    m_sampleDspInterfaceObserv = m_testRunner->findDspInterfaceByType(INJECT_RANGE_OBSERV);
    m_sampleDspInterfaceAdj = m_testRunner->findDspInterfaceByType(INJECT_RANGE_ADJUST);

    ControllerPersitentData::getData().m_criticalStatus = 0;
}

void test_range_hardware_overload::cleanup()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

void test_range_hardware_overload::noRangeAutoNoRangeGroupingNoActualValues()
{
    m_testRunner->setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testRunner->setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);

    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::all, "com5003d");
    //sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    //sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    sniffer.addComment("Before overload");
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Overload IL1");
    ControllerPersitentData::getData().m_criticalStatus = (1<<overloadInterrupt_I1);
    TimeMachineForTest::getInstance()->processTimers(501);
    TimeMachineForTest::getInstance()->processTimers(501);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/noRangeAutoNoRangeGroupingNoActualValues.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_hardware_overload::noRangeAutoNoRangeGroupingWithActualValues()
{
    m_testRunner->setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testRunner->setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);

    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::all, "com5003d");
    //sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    //sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    sniffer.addComment("Before overload");
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Overload IL1");
    ControllerPersitentData::getData().m_criticalStatus = (1<<overloadInterrupt_I1);
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Actual values");
    fireActualValues(m_sampleDspInterfaceProg, 5, 0.1, 50);
    TimeMachineForTest::getInstance()->processTimers(501);
    TimeMachineForTest::getInstance()->processTimers(501);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/noRangeAutoNoRangeGroupingWithActualValues.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_hardware_overload::noRangeAutoNoRangeGroupingWithActualValuesChangeRange()
{
    m_testRunner->setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testRunner->setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);

    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::all, "com5003d");
    //sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    //sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    sniffer.addComment("Before overload");
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Overload IL1");
    ControllerPersitentData::getData().m_criticalStatus = (1<<overloadInterrupt_I1);
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Actual values");
    fireActualValues(m_sampleDspInterfaceProg, 5, 0.1, 50);
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Change range");
    m_testRunner->setVfComponent(rangeEntityId, UL1RangeComponent, "8V");
    TimeMachineForTest::getInstance()->processTimers(501);

    sniffer.addComment("Actual values");
    fireActualValues(m_sampleDspInterfaceProg, 4, 0.1, 50);
    TimeMachineForTest::getInstance()->processTimers(501);
    TimeMachineForTest::getInstance()->processTimers(501);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/noRangeAutoNoRangeGroupingWithActualValuesChangeRange.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_hardware_overload::fireActualValues(const TestDspInterfacePtr &dspInterfaceProg, double uRms, double iRms, double frequency)
{
    DemoDspValuesRange rangeValues(8);
    rangeValues.setFrequency(frequency);
    rangeValues.setRmsPeakDCValue(0, uRms); // m0 / UL1
    rangeValues.setRmsPeakDCValue(1, uRms); // m1 / UL2
    rangeValues.setRmsPeakDCValue(2, uRms); // m2 / UL3
    rangeValues.setRmsPeakDCValue(3, iRms); // m3 / IL1
    rangeValues.setRmsPeakDCValue(4, iRms); // m4 / IL2
    rangeValues.setRmsPeakDCValue(5, iRms); // m5 / IL3
    rangeValues.setRmsPeakDCValue(6, uRms); // m6 / UAUX
    rangeValues.setRmsPeakDCValue(7, 0);    // m7 / IAUX
    dspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
}
