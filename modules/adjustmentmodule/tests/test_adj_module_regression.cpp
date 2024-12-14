#include "test_adj_module_regression.h"
#include "modulemanagertestrunner.h"
#include "adjustmentmodule.h"
#include <vs_storageeventsystem.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <testloghelpers.h>
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_regression)

static int constexpr rangeEntityId = 1020;
static int constexpr rmsEntityId = 1040;
static int constexpr dftEntityId = 1050;
static int constexpr fftEntityId = 1060;
static int constexpr adjEntityId = 1190;
static int constexpr scpiEntityId = 9999;

void test_adj_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
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
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");;
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    // just dump adjustment module to reduce FF on changing other modules
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << adjEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_adj_module_regression::dspInterfacesChange()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), DSP_INTERFACE_COUNT);
}

void test_adj_module_regression::adjInitWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:init IL1,10A;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_regression::adjInitWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:init IL1,10A;|*stb?");
    QCOMPARE(response, "+0"); // Init seem to work without permission...
}
