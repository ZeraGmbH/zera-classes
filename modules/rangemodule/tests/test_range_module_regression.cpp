#include "test_range_module_regression.h"
#include "modulemanagertestrunner.h"
#include "demovaluesdsprange.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <testloghelpers.h>
#include <vs_dumpjson.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_range_module_regression)

static int constexpr rangeEntityId = 1020;

void test_range_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(rangeEntityId));
}

void test_range_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(rangeEntityId));
}

void test_range_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rangeEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

enum dspInterfaces{
    RangeObsermatic,
    AdjustManagement,
    RangeModuleMeasProgram
};
static constexpr int rangeChannelCount = 8;
static constexpr int rangeRmsValuesCount = 8;
static constexpr int rangePeakValuesCount = 8;
static constexpr int rangeRawPeakValuesCount = 8;
static constexpr int rangeFrequencyCount = 1;
static constexpr int rangeResultCount = rangeRmsValuesCount + rangePeakValuesCount + rangeRawPeakValuesCount + rangeFrequencyCount;

void test_range_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    QCOMPARE(dspInterfaces[dspInterfaces::RangeObsermatic]->getValueList().size(), rangeChannelCount);
    QCOMPARE(dspInterfaces[dspInterfaces::AdjustManagement]->getValueList().size(), rangeChannelCount);
    QCOMPARE(dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->getValueList().size(), rangeChannelCount);
}

void test_range_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i);

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpActual.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rangeEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_range_module_regression::injectActualValuesWithPreScaling()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QCOMPARE(spyDspWrite.at(0).at(0), "GainCorrection");
    QCOMPARE(spyDspWrite.at(1).at(0), "PhaseCorrection");
    QCOMPARE(spyDspWrite.at(2).at(0), "OffsetCorrection");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpActual-preScaled2.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rangeEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_range_module_regression::injectActualValuesWithCheatingDisabled()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 0);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QVariant arguments = spyDspWrite[0][1];
    QVector<float> writtenCorrData = arguments.value<QVector<float>>();
    QVector<float> expectedGainCorr = {1, 1, 1, 1, 1, 1, 1, 1};
    //Extract first 8 values. They are corresponding to 8 channels, others are not used
    QVector<float> actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_module_regression::injectActualValuesWithCheatingEnabled()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QVariant arguments = spyDspWrite[0][1];
    QVector<float> writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 3, IL2 = 4, IL3 = 5, UAUX =6, IAUX = 7}
    //Gain correction channel sequence : {UL1, IL1, .....}
    QVector<float> expectedGainCorr = {1e-10, 1, 1e-10, 1, 1e-10, 1, 1, 1};
    //Extract first 8 values. They are corresponding to 8 channels, others are not used
    QVector<float> actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_module_regression::injectActualValuesWithCheatingAndRangeChanged()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QVariant arguments = spyDspWrite[0][1];
    QVector<float> writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    QVector<float> expectedGainCorr = {1e-10, 1, 1e-10, 1, 1e-10, 1, 1, 1};
    QVector<float> actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);

    testRunner.setVfComponent(rangeEntityId, "PAR_Channel1Range", "8V");
    spyDspWrite.clear();
    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    arguments = spyDspWrite[0][1];
    writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3 = 0.16V / UAUX = 5V , IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 3, IL2 = 4, IL3 = 5, UAUX =6, IAUX = 7}
    //Gain correction channel sequence : {UL1, IL1, .....}
    expectedGainCorr = {1e-10, 1, 1, 1, 1, 1, 1, 1};
    actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_module_regression::injectIncreasingActualValuesWithCheatingEnabled()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 2);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 0.1);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QVariant arguments = spyDspWrite[0][1];
    QVector<float> writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    //RMS Values 0.1 for all channels
    QVector<float> expectedGainCorr = {1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1e-10, 1};
    QVector<float> actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);

    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 10);

    spyDspWrite.clear();
    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    arguments = spyDspWrite[0][1];
    writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3-AUX = 5V, IL1-2-3 = 0.2A / IAUX ≈ 0
    //RMS Values 10 for all channels
    //Gain correction channel sequence : {UL1, IL1, .....}
    expectedGainCorr = {1, 1, 1, 1, 1, 1, 1, 1};
    actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_module_regression::injectActualValuesCheatingEnabledWithPreScaling()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValuesOnOff", 1);
    testRunner.setVfComponent(rangeEntityId, "PAR_IgnoreRmsValues", 0.5);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();

    DemoValuesDspRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, i*2.0);

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    QSignalSpy spyDspWrite(dspInterfaces[dspInterfaces::AdjustManagement].get(), &MockDspInterface::sigDspMemoryWrite);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspWrite.count(), 3);
    QVariant arguments = spyDspWrite[0][1];
    QVector<float> writtenCorrData = arguments.value<QVector<float>>();
    //Threshold is UL1-2-3-AUX = 1.25V , IL1-2-3 = 0.05A / IAUX ≈ 0
    //Gain correction channel sequence : {UL1, IL1, .....}
    //RMS Values {UL1 = 0, UL2 = 2, UL3 = 4, IL1 = 6, IL2 = 8, IL3 = 10, UAUX =12, IAUX = 14}
    //unscaled RMS Values ( UL1-UL2-UL3 / PAR_PreScalingGroup0) {UL1 = 0, UL2 = 1, UL3 = 2, IL1 = 6, IL2 = 8, IL3 = 10, UAUX =12, IAUX = 14}
    QVector<float> expectedGainCorr = {1e-10, 1, 1e-10, 1, 1, 1, 1, 1};
    QVector<float> actualGainCorr = writtenCorrData.mid(0, 8);
    QCOMPARE(expectedGainCorr, actualGainCorr);
}

void test_range_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    // dumping SCALEMEM:GAINCORRECTION2 does not seem reproducable yet
    QString obsermaticDumped = TestLogHelpers::dump(dspInterfaces[dspInterfaces::RangeObsermatic]->dumpAll());
    QString obsermaticExpected = TestLogHelpers::loadFile(":/dspDumps/dumpObsermatic.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(obsermaticExpected, obsermaticDumped));

    QString adjustListDumped = TestLogHelpers::dump(dspInterfaces[dspInterfaces::AdjustManagement]->dumpAll(true));
    QString adjustListExpected = TestLogHelpers::loadFile(":/dspDumps/dumpAdjustManagement.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(adjustListExpected, adjustListDumped));

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

