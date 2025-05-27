#include "test_source_full.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_source_full)

static int constexpr sourceEntityId = 1300;

void test_source_full::entity_avail_mt310s2()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json", false, "mt310s2");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(sourceEntityId));
}

void test_source_full::entity_avail_mt581s2()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json", false, "mt581s2");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(sourceEntityId));
}

void test_source_full::vein_dump_mt310s2()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json", false, "mt310s2");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitialMt310s2.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();

    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << sourceEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_source_full::vein_dump_mt581s2()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json", false, "mt581s2");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitialMt581s2.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();

    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << sourceEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}
