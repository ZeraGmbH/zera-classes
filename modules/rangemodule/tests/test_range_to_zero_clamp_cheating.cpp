#include "test_range_to_zero_clamp_cheating.h"
#include "modulemanagertestrunner.h"
#include "demodspvaluesrange.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_range_to_zero_clamp_cheating)

static int constexpr rangeEntityId = 1020;

void test_range_to_zero_clamp_cheating::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

static constexpr int rangeChannelCount = 8;

// 'Cheating' means: when the measured RMS value is below a configured
// threshold, the gain correction written to the DSP is clamped to
// almost zero (factor 1e-10). This was introduced for comparison measurements
// with old devices doin same.

void test_range_to_zero_clamp_cheating::injectActualValuesWithCheating()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    TestDspInterfacePtr sampleDspInterfaceProg = testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    TestDspInterfacePtr sampleDspInterfaceAdj = testRunner.findDspInterfaceByType(INJECT_RANGE_ADJUST);

    DemoDspValuesRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, i);


    // cheating disabled
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 0);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    QSignalSpy spyDspVarWriteDisabled(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWriteDisabled .count(), 3);
    TestDspVarWriteData writtenVarDataDisabled = spyDspVarWriteDisabled[0][0].value<TestDspVarWriteData>();
    QCOMPARE(writtenVarDataDisabled.varName, "GAINCORRECTION");
    QVector<float> expectedGainCorr = {1, 1, 1, 1, 1, 1, 1, 1};
    //Extract first 8 values. They are corresponding to 8 channels, others are not used
    QVector<float> actualGainCorr = writtenVarDataDisabled.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);


    // cheating enabled
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    QSignalSpy spyDspVarWriteEnabled(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWriteEnabled.count(), 3);
    TestDspVarWriteData writtenVarDataEnabled = spyDspVarWriteEnabled[0][0].value<TestDspVarWriteData>();
    QCOMPARE(writtenVarDataEnabled.varName, "GAINCORRECTION");
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 3, IL2 = 4, IL3 = 5, UAUX =6, IAUX = 7}
    //Gain correction channel sequence : {UL1, IL1, .....}
    expectedGainCorr = {1e-10, 1, 1e-10, 1, 1e-10, 1, 1, 1};
    //Extract first 8 values. They are corresponding to 8 channels, others are not used
    actualGainCorr = writtenVarDataEnabled.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_to_zero_clamp_cheating::injectActualValuesWithCheatingAndRangeChanged()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    TestDspInterfacePtr sampleDspInterfaceProg = testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    TestDspInterfacePtr sampleDspInterfaceAdj = testRunner.findDspInterfaceByType(INJECT_RANGE_ADJUST);

    DemoDspValuesRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, i);

    QSignalSpy spyDspVarWrite(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    TestDspVarWriteData writtenVarData = spyDspVarWrite[0][0].value<TestDspVarWriteData>();
    QCOMPARE(writtenVarData.varName, "GAINCORRECTION");
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    QVector<float> expectedGainCorr = {1e-10, 1, 1e-10, 1, 1e-10, 1, 1, 1};
    QVector<float> actualGainCorr = writtenVarData.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);

    testRunner.setVfComponent(rangeEntityId, "PAR_Channel1Range", "8V");
    spyDspVarWrite.clear();
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    writtenVarData = spyDspVarWrite[0][0].value<TestDspVarWriteData>();
    QCOMPARE(writtenVarData.varName, "GAINCORRECTION");
    //Threshold is UL1-2-3 = 0.16V / UAUX = 5V , IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 3, IL2 = 4, IL3 = 5, UAUX =6, IAUX = 7}
    //Gain correction channel sequence : {UL1, IL1, .....}
    expectedGainCorr = {1e-10, 1, 1, 1, 1, 1, 1, 1};
    actualGainCorr = writtenVarData.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_to_zero_clamp_cheating::injectIncreasingActualValuesWithCheatingEnabled()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    TestDspInterfacePtr sampleDspInterfaceProg = testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    TestDspInterfacePtr sampleDspInterfaceAdj = testRunner.findDspInterfaceByType(INJECT_RANGE_ADJUST);

    DemoDspValuesRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, 0.1);

    QSignalSpy spyDspVarWrite(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    TestDspVarWriteData writtenVarData = spyDspVarWrite[0][0].value<TestDspVarWriteData>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    //RMS Values 0.1 for all channels
    QVector<float> expectedGainCorr = {1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1};
    QVector<float> actualGainCorr = writtenVarData.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);

    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, 10);

    spyDspVarWrite.clear();
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    writtenVarData = spyDspVarWrite[0][0].value<TestDspVarWriteData>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values 10 for all channels
    //Gain correction channel sequence : {UL1, IL1, .....}
    expectedGainCorr = {1, 1, 1, 1, 1, 1, 1, 1};
    actualGainCorr = actualGainCorr = writtenVarData.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_to_zero_clamp_cheating::injectActualValuesCheatingEnabledWithPreScaling()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 0.5);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1");

    TestDspInterfacePtr sampleDspInterfaceProg = testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    TestDspInterfacePtr sampleDspInterfaceAdj = testRunner.findDspInterfaceByType(INJECT_RANGE_ADJUST);

    DemoDspValuesRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, i*2.0);

    QSignalSpy spyDspVarWrite(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    TestDspVarWriteData writtenVarData = spyDspVarWrite[0][0].value<TestDspVarWriteData>();
    //Threshold is UL1-2-3-AUX = 1.25V , IL1-2-3 = 0.05A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    //RMS Values {UL1 = 0, UL2 = 2, UL3 = 4, IL1 = 6, IL2 = 8, IL3 = 10, UAUX =12, IAUX = 14}
    //unscaled RMS Values ( UL1-UL2-UL3 / PAR_PreScalingGroup0) {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 6, IL2 = 8, IL3 = 10, UAUX =12, IAUX = 14}
    QVector<float> expectedGainCorr = {1e-10, 1, 1e-10, 1, 1, 1, 1, 1};
    QVector<float> actualGainCorr = writtenVarData.values.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}
