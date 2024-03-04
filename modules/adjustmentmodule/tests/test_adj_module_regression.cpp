#include "test_adj_module_regression.h"
#include "modulemanagertestrunner.h"
#include "vs_veinhash.h"
#include <QTest>

QTEST_MAIN(test_adj_module_regression)

static int constexpr rangeEntityId = 1020;
static int constexpr rmsEntityId = 1040;
static int constexpr dftEntityId = 1050;
static int constexpr fftEntityId = 1060;
static int constexpr adjEntityId = 1190;

void test_adj_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 6);
    QVERIFY(storageHash->hasEntity(rangeEntityId));
    QVERIFY(storageHash->hasEntity(rmsEntityId));
    QVERIFY(storageHash->hasEntity(dftEntityId));
    QVERIFY(storageHash->hasEntity(fftEntityId));
    QVERIFY(storageHash->hasEntity(adjEntityId));
}
