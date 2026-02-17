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

    TestDspInterfacePtr dftDspInterface = testRunner.getDspInterface(periodAverageEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dftDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-all-channels-maxperiod-5.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_periodaverage_module::dumpDspSetupAuxChannels13PeriodsMax()
{
    ModuleManagerTestRunner testRunner(":/sessions/aux-channels-maxperiod-13.json");

    TestDspInterfacePtr dftDspInterface = testRunner.getDspInterface(periodAverageEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dftDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-aux-channels-maxperiod-13.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}
