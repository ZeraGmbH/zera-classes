#include "test_power1_module_regression.h"
#include "modulemanagertestrunner.h"
#include "power1modulemeasprogram.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <vs_dumpjson.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <timemachineobject.h>
#include <QJsonArray>
#include <QTest>

QTEST_MAIN(test_power1_module_regression)

const int powerEntityId = 1070;

void test_power1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();

    QList<int> entityList = storageDb->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageDb->hasEntity(powerEntityId));
}

void test_power1_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/session-power1-test.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_power1_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/session-power1-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> powerValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powerValues.append(i);
    powerValues.append(powerValues[0] + powerValues[1] + powerValues[2]);

    dspInterfaces[0]->fireActValInterrupt(powerValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpActual.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_power1_module_regression::testScpiCommandsDisabled()
{
    ModuleManagerTestRunner testRunner(":/session-power1-withoutScpi-test.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial-withoutScpi.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(storageDb, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));

    //When we change into 'INF_ModuleInterface', we take current 'jsonDumped' and copy it to test-data.
    //So, here is an extra check to be sure that no scpi commands are introduced by accident in future.
    QJsonObject dumpedModuleInterface = QJsonDocument::fromVariant(storageDb->getStoredValue(powerEntityId, QString("INF_ModuleInterface"))).object();
    QJsonValue scpiCmds = dumpedModuleInterface.value("SCPIInfo").toObject().value("Cmd");
    QCOMPARE(scpiCmds.toArray().count(), 0);
}

void test_power1_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

void test_power1_module_regression::dumpDspOnMeasModeChange()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    setMeasMode(testRunner.getVfCmdEventHandlerSystemPtr(), "4LW", "3LW");
    setMeasMode(testRunner.getVfCmdEventHandlerSystemPtr(), "3LW", "2LW");
    setMeasMode(testRunner.getVfCmdEventHandlerSystemPtr(), "2LW", "4LW");

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-measmode-change.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

void test_power1_module_regression::setMeasMode(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                                                QString measModeOld,
                                                QString measModeNew)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(powerEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_MeasuringMode", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(measModeOld, measModeNew);
    TimeMachineObject::feedEventLoop();
}

