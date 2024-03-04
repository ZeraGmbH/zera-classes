#include "test_adj_module_regression.h"
#include "modulemanagertestrunner.h"
#include "vs_veinhash.h"
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_adj_module_regression)

static int constexpr rangeEntityId = 1020;
static int constexpr rmsEntityId = 1040;
static int constexpr dftEntityId = 1050;
static int constexpr fftEntityId = 1060;
static int constexpr adjEntityId = 1190;
const QList<int> test_adj_module_regression::m_entityIdList =
    QList<int>() << rangeEntityId << rmsEntityId << dftEntityId << fftEntityId << adjEntityId;


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

void test_adj_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, m_entityIdList);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}
