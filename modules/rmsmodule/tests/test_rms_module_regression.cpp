#include "test_rms_module_regression.h"
#include "demovaluesdsprms.h"
#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <QBuffer>
#include <QTest>

QTEST_MAIN(test_rms_module_regression)

static int constexpr rmsEntityId = 1040;

void test_rms_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(rmsEntityId));
}

void test_rms_module_regression::moduleConfigFromResource()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(rmsEntityId));
}

void test_rms_module_regression::veinDumpInitial()
{
    QFile file(":/dumpInitial.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    VeinStorage::DumpJson::dumpToFile(veinStorage->getDb(),&buff, QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
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

    dspInterfaces[0]->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpActual.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    VeinStorage::DumpJson::dumpToFile(veinStorage->getDb(),&buff, QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_rms_module_regression::injectActualTwice()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QVector<float> actValues(rmsResultCount);

    actValues[1] = 37;
    dspInterfaces[0]->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(veinStorage->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(37.0));

    actValues[1] = 42;
    dspInterfaces[0]->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(veinStorage->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(42.0));
}

void test_rms_module_regression::injectSymmetricValues()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    RMSMODULE::cRmsModule *rmsModule = static_cast<RMSMODULE::cRmsModule*>(testRunner.getModule("rmsmodule", rmsEntityId));
    RMSMODULE::cRmsModuleConfiguration config;
    config.setConfiguration(rmsModule->getConfiguration());

    DemoValuesDspRms demoDspValue(config.getConfigurationData()->m_valueChannelList);
    demoDspValue.setAllValuesSymmetric(230, 5);
    dspInterfaces[0]->fireActValInterrupt(demoDspValue.getDspValues(), 0 /* dummy */);
    TimeMachineObject::feedEventLoop();

    QFile file(":/dumpSymmetric.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();

    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped;
    QBuffer buff(&jsonDumped);
    VeinStorage::DumpJson::dumpToFile(veinStorage->getDb(),&buff, QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}
