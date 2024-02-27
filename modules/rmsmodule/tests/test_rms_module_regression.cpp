#include "test_rms_module_regression.h"
#include "rmsmodulemeasprogram.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_rms_module_regression)

static int constexpr rmsEntityId = 1040;

void test_rms_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(rmsEntityId));
}

void test_rms_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(rmsEntityId));
}

void test_rms_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << rmsEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int currentPhaseCount = 4;
static constexpr int rmsResultCount = voltagePhaseNeutralCount + voltagePhasePhaseCount + currentPhaseCount;

void test_rms_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QStringList valueList = dspInterfaces[0]->getValueList();
    QCOMPARE(valueList.count(), rmsResultCount);
}

void test_rms_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> actValues(rmsResultCount);
    for(int i = 0; i<rmsResultCount; i++)
        actValues[i] = i;

    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << rmsEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

void test_rms_module_regression::injectActualTwice()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QVector<float> actValues(rmsResultCount);

    actValues[1] = 37;
    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(storageHash->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(37.0));

    actValues[1] = 42;
    dspInterfaces[0]->fireActValInterrupt(actValues, irqNr);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(storageHash->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(42.0));
}
