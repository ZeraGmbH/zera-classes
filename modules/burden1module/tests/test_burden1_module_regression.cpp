#include "test_burden1_module_regression.h"
#include "modulemanagertestrunner.h"
#include "burden1measdelegate.h"
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_burden1_module_regression)

static int constexpr dftEntityId = 1050;
static int constexpr dftBurdenCurrentId = 1160;
static int constexpr dftBurdenVoltageId = 1161;

void test_burden1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 4);
    QVERIFY(storageHash->hasEntity(dftEntityId));
    QVERIFY(storageHash->hasEntity(dftBurdenCurrentId));
    QVERIFY(storageHash->hasEntity(dftBurdenVoltageId));
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
    storageHash->dumpToFile(&buff, QList<int>() << dftBurdenCurrentId << dftBurdenVoltageId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

void test_burden1_module_regression::resistanceCalculation()
{
    constexpr double conductanceCopper = 56.0; // from doc

    double length = 100.0;
    double crossSection = 2.5;
    double expected = length / (crossSection * conductanceCopper); // from doc
    QCOMPARE(BURDEN1MODULE::cBurden1MeasDelegate::calcWireResistence(length, crossSection), expected);

    length = 10.0;
    crossSection = 10.0;
    expected = length / (crossSection * conductanceCopper);
    QCOMPARE(BURDEN1MODULE::cBurden1MeasDelegate::calcWireResistence(length, crossSection), expected);
}
