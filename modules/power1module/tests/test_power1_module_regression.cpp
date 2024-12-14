#include "test_power1_module_regression.h"
#include "modulemanagertestrunner.h"
#include "power1modulemeasprogram.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <vs_dumpjson.h>
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

void test_power1_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-power1-test.json");
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

