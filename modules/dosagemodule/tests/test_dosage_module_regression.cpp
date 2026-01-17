#include "test_dosage_module_regression.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_dosage_module_regression)

static int constexpr rmsEntityId = 1040;
static int constexpr power1module1EntityId = 1070;
static int constexpr dosageEntityId = 1600;

void test_dosage_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/def-session-maximal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();
    QCOMPARE(entityList.count(), 4);
    QVERIFY(veinStorageDb->hasEntity(rmsEntityId));
    QVERIFY(veinStorageDb->hasEntity(power1module1EntityId));
    QVERIFY(veinStorageDb->hasEntity(dosageEntityId));
}

void test_dosage_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/def-session-maximal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << dosageEntityId);
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}
