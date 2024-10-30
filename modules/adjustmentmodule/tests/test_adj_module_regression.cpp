#include "test_adj_module_regression.h"
#include "modulemanagertestrunner.h"
#include "adjustmentmodule.h"
#include <vs_veinhash.h>
#include <testloghelpers.h>
#include <testloghelpers.h>
#include <scpimoduleclientblocked.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_adj_module_regression)

static int constexpr rangeEntityId = 1020;
static int constexpr rmsEntityId = 1040;
static int constexpr dftEntityId = 1050;
static int constexpr fftEntityId = 1060;
static int constexpr adjEntityId = 1190;
static int constexpr scpiEntityId = 9999;

void test_adj_module_regression::initTestCase()
{
    TestModuleManager::enableTests();
}

void test_adj_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();
    QCOMPARE(entityList.count(), 7);
    QVERIFY(veinStorage->hasEntity(rangeEntityId));
    QVERIFY(veinStorage->hasEntity(rmsEntityId));
    QVERIFY(veinStorage->hasEntity(dftEntityId));
    QVERIFY(veinStorage->hasEntity(fftEntityId));
    QVERIFY(veinStorage->hasEntity(adjEntityId));
    QVERIFY(veinStorage->hasEntity(scpiEntityId));
}

void test_adj_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    // just dump adjustment module to reduce FF on changing other modules
    veinStorage->dumpToFile(&buff, QList<int>() << adjEntityId);

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
