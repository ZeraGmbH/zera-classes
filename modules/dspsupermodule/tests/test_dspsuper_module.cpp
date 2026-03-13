#include "test_dspsuper_module.h"
#include "modulemanagertestrunner.h"
#include <zdspdumpfunctions.h>
#include <timemachineobject.h>
#include <vf_core_stack_client.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_dspsuper_module)

static int constexpr dspSuperEntityId = 9000;

void test_dspsuper_module::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_dspsuper_module::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();

    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorageDb->hasEntity(dspSuperEntityId));
}

void test_dspsuper_module::dspDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    TestDspInterfacePtr dspInterface = testRunner.getDspInterface(dspSuperEntityId);
    QString measProgramDumped = TestLogHelpers::dump(dspInterface->dumpAll());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspDumps/dumpInitial.json", measProgramDumped));
}

void test_dspsuper_module::zdspDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    ZDspServer* server = testRunner.getDspServer();
    QString dumped = TestLogHelpers::dump(ZDspDumpFunctions::getFullDump(server));
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/zdspDumps/dumpInitial.json", dumped));
}

void test_dspsuper_module::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray dumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << dspSuperEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/dumpInitial.json", dumped));
}
