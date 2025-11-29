#include "test_adj_module_regression.h"
#include <vs_storageeventsystem.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_adj_module_regression)

static int constexpr rangeEntityId = 1020;
static int constexpr rmsEntityId = 1040;
static int constexpr dftEntityId = 1050;
static int constexpr fftEntityId = 1060;
static int constexpr adjEntityId = 1190;
static int constexpr scpiEntityId = 9999;

void test_adj_module_regression::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-minimal-dc.json", true);
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_adj_module_regression::destroyCommonTestRunner()
{
    m_scpiClient = nullptr;
    m_testRunner = nullptr;
}

void test_adj_module_regression::minimalSession()
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 7);
    QVERIFY(veinStorage->getDb()->hasEntity(rangeEntityId));
    QVERIFY(veinStorage->getDb()->hasEntity(rmsEntityId));
    QVERIFY(veinStorage->getDb()->hasEntity(dftEntityId));
    QVERIFY(veinStorage->getDb()->hasEntity(fftEntityId));
    QVERIFY(veinStorage->getDb()->hasEntity(adjEntityId));
    QVERIFY(veinStorage->getDb()->hasEntity(scpiEntityId));
}

void test_adj_module_regression::veinDumpInitial()
{
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");;
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    // just dump adjustment module to reduce FF on changing other modules
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << adjEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_adj_module_regression::adjInitWithPermission()
{
    QString response = m_scpiClient->sendReceive("calc:adj1:init IL1,10A;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_regression::adjInitWithoutPermission()
{
    destroyCommonTestRunner();
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    ScpiModuleClientBlocked scpiClient;

    QString response = scpiClient.sendReceive("calc:adj1:init IL1,10A;|*stb?");
    QCOMPARE(response, "+0"); // Init seem to work without permission...
}
