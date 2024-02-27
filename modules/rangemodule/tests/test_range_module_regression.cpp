#include "test_range_module_regression.h"
#include "rangemodulemeasprogram.h"
#include "modulemanagertestrunner.h"
#include "vf_client_component_setter.h"
#include "vf_entity_component_event_item.h"
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

    QVector<float> actValues(rangeResultCount);
    for(int i = 0; i < rangeResultCount; i++)
        actValues[i] = i;

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(actValues, irqNr);
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
    setVfPreScaling(testRunner.getVfCmdEventHandlerSystemPtr(), "2/1");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 3);

    QVector<float> actValues(rangeResultCount);
    for(int i = 0; i < rangeResultCount; i++)
        actValues[i] = i;

    TimeMachineForTest::getInstance()->processTimers(500); //for 'm_AdjustTimer'
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(actValues, irqNr);
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

void test_range_module_regression::setVfPreScaling(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QVariant value)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(rangeEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_PreScalingEnabledGroup0", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(false, true);
    TimeMachineObject::feedEventLoop();

    setter = VfClientComponentSetter::create("PAR_PreScalingGroup0", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent("1/1", value);
    TimeMachineObject::feedEventLoop();
}

