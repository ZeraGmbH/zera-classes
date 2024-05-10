#include "test_power1_module_regression.h"
#include "modulemanagertestrunner.h"
#include "power1modulemeasprogram.h"
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_power1_module_regression)

const int powerEntityId = 1070;

void test_power1_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->hasEntity(powerEntityId));
}

void test_power1_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-power1-test.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->hasEntity(powerEntityId));
}

void test_power1_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-power1-test.json");
    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    veinStorage->dumpToFile(&buff, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_power1_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/session-power1-test.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> powerValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powerValues.append(i);
    powerValues.append(powerValues[0] + powerValues[1] + powerValues[2]);

    dspInterfaces[0]->fireActValInterrupt(powerValues, irqNr);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    VeinEvent::StorageSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    veinStorage->dumpToFile(&buff, QList<int>() << powerEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

