#include "test_periodaverage_module.h"
#include "demovaluesdspperiodaverage.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <vf_core_stack_client.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_periodaverage_module)

static int constexpr periodAverageEntityId = 1055;

void test_periodaverage_module::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_periodaverage_module::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();

    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorageDb->hasEntity(periodAverageEntityId));
}

void test_periodaverage_module::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitial.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << periodAverageEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_periodaverage_module::veinDumpInitialAllChannels5PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/all-channels-maxperiod-5.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dump-all-channels-maxperiod-5.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << periodAverageEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_periodaverage_module::veinDumpInitialAuxChannels13PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/aux-channels-maxperiod-13.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dump-aux-channels-maxperiod-13.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << periodAverageEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_periodaverage_module::dumpDspSetupAllChannels5PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/all-channels-maxperiod-5.json");

    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(periodAverageEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-all-channels-maxperiod-5.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_periodaverage_module::dumpDspSetupAuxChannels13PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/aux-channels-maxperiod-13.json");

    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(periodAverageEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-aux-channels-maxperiod-13.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}


void test_periodaverage_module::injectValuesAllChannels5PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/all-channels-maxperiod-5.json");

    constexpr int maxPeriods = 5;
    constexpr int measPeriods = 5;
    const QStringList mtChannelsAll = QStringList() << "m0" << "m1" << "m2" << "m3" << "m4" << "m5" << "m6" << "m7";
    DemoValuesDspPeriodAverage values(mtChannelsAll, maxPeriods, measPeriods);
    values.setValue("m0", 0, 41.8);
    values.setValue("m0", 1, 41.9);
    values.setValue("m0", 2, 42);
    values.setValue("m0", 3, 42.1);
    values.setValue("m0", 4, 42.2);
    values.setValue("m1", 0, 36.6);
    values.setValue("m1", 1, 36.8);
    values.setValue("m1", 2, 37);
    values.setValue("m1", 3, 37.2);
    values.setValue("m1", 4, 37.4);
    QVector<float> dspValues = values.getDspValues();

    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(periodAverageEntityId);
    dspInterface->fireActValInterrupt(dspValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dump-all-channels-maxperiod-5-values-injected.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << periodAverageEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_periodaverage_module::injectValuesAuxChannels13PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/aux-channels-maxperiod-13.json");

    constexpr int maxPeriods = 13;
    constexpr int measPeriods = 13;
    const QStringList mtChannelsAux = QStringList() << "m6" << "m7";
    DemoValuesDspPeriodAverage values(mtChannelsAux, maxPeriods, measPeriods);
    values.setValue("m6", 0, 31.4);
    values.setValue("m6", 1, 31.5);
    values.setValue("m6", 2, 31.6);
    values.setValue("m6", 3, 31.7);
    values.setValue("m6", 4, 31.8);
    values.setValue("m6", 5, 31.9);
    values.setValue("m6", 6, 32);
    values.setValue("m6", 7, 32.1);
    values.setValue("m6", 8, 32.2);
    values.setValue("m6", 9, 32.3);
    values.setValue("m6", 10, 32.4);
    values.setValue("m6", 11, 32.5);
    values.setValue("m6", 12, 32.6);

    values.setValue("m7", 0, 45.8);
    values.setValue("m7", 1, 46.0);
    values.setValue("m7", 2, 46.2);
    values.setValue("m7", 3, 46.4);
    values.setValue("m7", 4, 46.6);
    values.setValue("m7", 5, 46.8);
    values.setValue("m7", 6, 47);
    values.setValue("m7", 7, 47.2);
    values.setValue("m7", 8, 47.4);
    values.setValue("m7", 9, 47.6);
    values.setValue("m7", 10, 47.8);
    values.setValue("m7", 11, 48.0);
    values.setValue("m7", 12, 48.2);
    QVector<float> dspValues = values.getDspValues();

    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(periodAverageEntityId);
    dspInterface->fireActValInterrupt(dspValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dump-aux-channels-maxperiod-13-values-injected.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << periodAverageEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}
