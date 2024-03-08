#include "test_range_module_regression.h"
#include "modulemanagertestrunner.h"
#include "vf_client_component_setter.h"
#include "vf_entity_component_event_item.h"
#include "demovaluesdsprange.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <QSignalSpy>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_range_module_regression)

static int constexpr rangeEntityId = 1020;

void test_range_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(rangeEntityId));
}

void test_range_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(rangeEntityId));
}

void test_range_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << rangeEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
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

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << rangeEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

void test_range_module_regression::injectActualValuesWithPreScaling()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_PreScalingEnabledGroup0", false, true);
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_PreScalingGroup0", "1/1", "2/1");

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

    QFile file(":/dumpActual-preScaled2.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << rangeEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

void test_range_module_regression::injectActualValuesWithCheatingDisabled()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValuesOnOff", 0, 0);
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValues", 1, 2);

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
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValuesOnOff", 0, 1);
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValues", 1, 2);

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
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValuesOnOff", 0, 1);
    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_IgnoreRmsValues", 1, 2);

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

    setVfComponent(testRunner.getVfCmdEventHandlerSystemPtr(), "PAR_Channel1Range", "250V", "8V");
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

void test_range_module_regression::setVfComponent(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QString componentName, QVariant oldValue, QVariant newValue)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(rangeEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create(componentName, entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(oldValue, newValue);
    TimeMachineObject::feedEventLoop();
}
