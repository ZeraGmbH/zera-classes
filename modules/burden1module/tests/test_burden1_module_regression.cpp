#include "test_burden1_module_regression.h"
#include "modulemanagertestrunner.h"
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_burden1_module_regression)

static int constexpr dftEntityId = 1050;
static int constexpr dftBurden1Id = 1160;
static int constexpr dftBurden2Id = 1161;

void test_burden1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 4);
    QVERIFY(storageHash->hasEntity(dftEntityId));
    QVERIFY(storageHash->hasEntity(dftBurden1Id));
    QVERIFY(storageHash->hasEntity(dftBurden2Id));
}

void test_burden1_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << dftBurden1Id << dftBurden2Id);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}
