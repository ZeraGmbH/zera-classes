#include "test_range_scaling_factors.h"
#include "demodspvaluesrange.h"
#include "testservicesniffer.h"
#include "vf_client_component_setter.h"
#include <scpisingletransactionblocked.h>
#include <zscpi_response_definitions.h>
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
static const char* UL1RangeComponent("PAR_Channel1Range");
static const char* IL1RangeComponent("PAR_Channel4Range");
static const char* RangeAutomaticComponent("PAR_RangeAutomatic");
static const char* RangeGroupingComponent("PAR_ChannelGrouping");
static constexpr int channelCountInpected = 2;

void test_range_scaling_factors::init()
{
    TimeMachineForTest::reset();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-range-test.json", /*initialAdjPermission=*/true);
    m_sampleDspInterfaceProg = m_testRunner->findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    m_sampleDspInterfaceObserv = m_testRunner->findDspInterfaceByType(INJECT_RANGE_OBSERV);
    m_sampleDspInterfaceAdj = m_testRunner->findDspInterfaceByType(INJECT_RANGE_ADJUST);

    constexpr double coeff1 = 0.001;
    setCorrectionCoefficients("m0", "250V", "GAIN", 1, coeff1); // UL1
    setCorrectionCoefficients("m3", "10A", "GAIN", 1, coeff1); // IL1
    setCorrectionCoefficients("m0", "250V", "OFFSET", 1, coeff1); // UL1
    setCorrectionCoefficients("m3", "10A", "OFFSET", 1, coeff1); // IL1
    setCorrectionCoefficients("m0", "250V", "PHASE", 1, coeff1); // UL1
    setCorrectionCoefficients("m3", "10A", "PHASE", 1, coeff1); // IL1
    setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);
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
    setVfComponent(rangeEntityId, UL1RangeComponent, "8V");
    setVfComponent(rangeEntityId, IL1RangeComponent, "1.0A");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/serviceDumps/changeOneVoltageAndOneCurrentRange.json", sniffer.dump()));
}

void test_range_scaling_factors::preScalingUOnOff()
{
    TestServiceSniffer sniffer;
    //sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestServiceSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceProg, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceObserv, channelCountInpected);
    sniffer.sniffDspVarWrites(m_sampleDspInterfaceAdj, channelCountInpected);

    // Expectation
    TimeMachineForTest::getInstance()->processTimers(500); // enable correction / adjustment
    setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1*(1)");
    setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    fireActualValues(m_sampleDspInterfaceProg, 20, 1, 50); // we scale U only

    TimeMachineForTest::getInstance()->processTimers(500); // enable next correction / adjustment
    setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", false);
    fireActualValues(m_sampleDspInterfaceProg, 10, 1, 50);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/serviceDumps/preScalingUOnOff.json", sniffer.dump()));
}


QVariant test_range_scaling_factors::getVfComponent(int entityId, const QString &componentName)
{
    return m_testRunner->getVeinStorageDb()->getStoredValue(entityId, componentName);
}

void test_range_scaling_factors::setVfComponent(int entityId, const QString &componentName, const QVariant &newValue)
{
    QVariant oldValue = getVfComponent(entityId, componentName);
    QEvent* event = VfClientComponentSetter::generateEvent(entityId, componentName, oldValue, newValue);
    emit m_testRunner->getVeinStorageSystem()->sigSendEvent(event); // could be any event system
    TimeMachineObject::feedEventLoop();
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

void test_range_scaling_factors::setCorrectionCoefficients(const QString &channelMName,
                                                                 const QString &rangeName,
                                                                 const QString &correctionType, // "GAIN", "PHASE" or "OFFSET"
                                                                 int order,
                                                                 double value)
{
    QString scpiCmd = QString("SENS:%1:%2:CORR:%3:COEFFICIENT:%4")
    .arg(channelMName, rangeName, correctionType)
        .arg(order);
    QString answer = ScpiSingleTransactionBlocked::cmd(scpiCmd, QString::number(value, 'g', 12));
    QCOMPARE(answer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}
