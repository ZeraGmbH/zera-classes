#include "test_range_scaling_factors.h"
#include "testrangehelpers.h"
#include "demodspvaluesrange.h"
#include "testservicesniffer.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_range_scaling_factors)

void test_range_scaling_factors::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

static int constexpr rangeEntityId = 1020;
static const char* RangeAutomaticComponent("PAR_RangeAutomatic");
static const char* RangeGroupingComponent("PAR_ChannelGrouping");

static constexpr int channelCountInpected = 2;
static constexpr int DspChannelUl1 = 0; // m0
static const char* UL1RangeComponent("PAR_Channel1Range");
static const char* IL1RangeComponent("PAR_Channel4Range");

static constexpr double coeff1 = 0.001;

void test_range_scaling_factors::init()
{
    TimeMachineForTest::reset();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-range-test.json", /*initialAdjPermission=*/true);
    m_sampleDspInterfaceProg = m_testRunner->findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    m_sampleDspInterfaceObserv = m_testRunner->findDspInterfaceByType(INJECT_RANGE_OBSERV);
    m_sampleDspInterfaceAdj = m_testRunner->findDspInterfaceByType(INJECT_RANGE_ADJUST);

    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::GAIN, 1, coeff1); // UL1
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::PHASE, 1, coeff1);
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::OFFSET, 1, coeff1);
    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::GAIN, 1, coeff1); // IL1
    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::PHASE, 1, coeff1);
    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::OFFSET, 1, coeff1);
    m_testRunner->setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testRunner->setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);
}

void test_range_scaling_factors::cleanup()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

void test_range_scaling_factors::changeOneVoltageAndOneCurrentRange()
{
    TestServiceSniffer sniffer;
    //sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestServiceSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceProg, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceObserv, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceAdj, channelCountInpected);

    TimeMachineForTest::getInstance()->processTimers(500);
    fireActualValues(m_sampleDspInterfaceProg, 5, 0.1, 50);

    TimeMachineForTest::getInstance()->processTimers(500);
    m_testRunner->setVfComponent(rangeEntityId, UL1RangeComponent, "8V");
    m_testRunner->setVfComponent(rangeEntityId, IL1RangeComponent, "1.0A");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/serviceDumps/changeOneVoltageAndOneCurrentRange.json", sniffer.dump()));
}

void test_range_scaling_factors::preScalingUOnOff()
{
    TestServiceSniffer sniffer;
    //sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestServiceSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceProg, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceObserv, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceAdj, channelCountInpected);

    constexpr double unscaledU = 10.0;
    constexpr double unscaledI = 1.0;
    // prescale on
    TimeMachineForTest::getInstance()->processTimers(500); // enable correction / adjustment
    QSignalSpy spyDspVarWriteOn(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    m_testRunner->setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1*(1)");
    m_testRunner->setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    fireActualValues(m_sampleDspInterfaceProg, unscaledU*2, unscaledI, 50); // we scale U only
    float gainUScaledOn = TestRangeHelpers::findDspWriteValue(spyDspVarWriteOn, "GAINCORRECTION", DspChannelUl1);

    // prescale off
    TimeMachineForTest::getInstance()->processTimers(500); // enable next correction / adjustment
    QSignalSpy spyDspVarWriteOff(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    m_testRunner->setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", false);
    fireActualValues(m_sampleDspInterfaceProg, unscaledU, unscaledI, 50);
    float gainUScaledOff = TestRangeHelpers::findDspWriteValue(spyDspVarWriteOn, "GAINCORRECTION", DspChannelUl1);

    double gainUexpected = TestRangeHelpers::testPolynom(unscaledU, 1, coeff1);
    QCOMPARE(gainUScaledOff, gainUexpected);
    QCOMPARE(gainUScaledOn, gainUexpected);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/serviceDumps/preScalingUOnOff.json", sniffer.dump()));
}


void test_range_scaling_factors::fireActualValues(const TestDspInterfacePtr &dspInterfaceProg,
                                                  double uRms, double iRms, double frequency)
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
    TimeMachineObject::feedEventLoop();
}
