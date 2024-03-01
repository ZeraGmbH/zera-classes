#include "test_fft_module_regression.h"
#include "demovaluesdspfft.h"
#include "fftmodulemeasprogram.h"
#include "testfactoryserviceinterfaces.h"
#include "modulemanagertestrunner.h"
#include <cmath>
#include <timemachineobject.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_fft_module_regression)

static int constexpr fftEntityId = 1060;

void test_fft_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json"); // moving window is off at the time of writing
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(fftEntityId));
}

void test_fft_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-from-resource.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(storageHash->hasEntity(fftEntityId));
}

void test_fft_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-from-resource.json");
    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << fftEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int currentPhaseCount = 4;
static constexpr int fftResultCount = voltagePhaseNeutralCount + currentPhaseCount;

void test_fft_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/session-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QStringList valueList = dspInterfaces[0]->getValueList();
    QCOMPARE(valueList.count(), fftResultCount);
}

void test_fft_module_regression::injectValues()
{
    ModuleManagerTestRunner testRunner(":/session-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    DemoValuesDspFft dspValues(dspInterfaces[0]->getValueList().count());
    dspValues.setValue(0, 0, 1, 1);
    dspValues.setValue(0, 1, 2, 0);
    dspValues.setValue(0, 2, 0, 3);

    dspValues.setValue(1, 0, 3, 5);
    dspValues.setValue(1, 1, 230, 0);
    dspValues.setValue(1, 2, 0, 42);

    dspValues.setValue(2, 0, 5, 5);
    dspValues.setValue(2, 1, 230/M_SQRT2, 230/M_SQRT2);
    dspValues.setValue(2, 2, 42/M_SQRT2, 42/M_SQRT2);

    dspValues.setValue(3, 0, 2, 2);
    dspValues.setValue(3, 1, 4, 0);
    dspValues.setValue(3, 2, 0, 6);

    dspValues.setValue(4, 0, 4, 4);
    dspValues.setValue(4, 1, 6, 0);
    dspValues.setValue(4, 2, 0, 8);

    dspValues.setValue(5, 0, 2, 2);
    dspValues.setValue(5, 1, 4/M_SQRT2, 4/M_SQRT2);
    dspValues.setValue(5, 2, 6/M_SQRT2, 6/M_SQRT2);

    dspInterfaces[0]->fireActValInterrupt(dspValues.getDspValues(), irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QString jsonExpected = file.readAll();

    VeinStorage::VeinHash* storageHash = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    storageHash->dumpToFile(&buff, QList<int>() << fftEntityId);

    if(jsonExpected != jsonDumped) {
        qWarning("Expected storage hash:");
        qInfo("%s", qPrintable(jsonExpected));
        qWarning("Dumped storage hash:");
        qInfo("%s", qPrintable(jsonDumped));
        QCOMPARE(jsonExpected, jsonDumped);
    }
}
