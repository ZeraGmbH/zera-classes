#include "test_rms_module_regression.h"
#include "demovaluesdsprms.h"
#include "rmsmodule.h"
#include "rmsmoduleconfiguration.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <timemachineobject.h>
#include <testloghelpers.h>
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

void test_rms_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int currentPhaseCount = 4;
static constexpr int rmsResultCount = voltagePhaseNeutralCount + voltagePhasePhaseCount + currentPhaseCount;

void test_rms_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    TestDspInterfacePtr rmsDspInterface = testRunner.getDspInterface(rmsEntityId);

    QStringList valueList = rmsDspInterface->getValueList();
    QCOMPARE(valueList.count(), rmsResultCount);
}

void test_rms_module_regression::injectActualValues()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    TestDspInterfacePtr rmsDspInterface = testRunner.getDspInterface(rmsEntityId);

    QVector<float> actValues(rmsResultCount);
    for(int i = 0; i<rmsResultCount; i++)
        actValues[i] = i;

    rmsDspInterface->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected= TestLogHelpers::loadFile(":/dumpActual.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_rms_module_regression::injectActualTwice()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    TestDspInterfacePtr rmsDspInterface = testRunner.getDspInterface(rmsEntityId);

    VeinStorage::AbstractDatabase* storageDb = testRunner.getVeinStorageSystem()->getDb();
    QVector<float> actValues(rmsResultCount);

    actValues[1] = 37;
    rmsDspInterface->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(storageDb->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(37.0));

    actValues[1] = 42;
    rmsDspInterface->fireActValInterrupt(actValues, 0 /* dummy */);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(storageDb->getStoredValue(rmsEntityId, "ACT_RMSPN2"), QVariant(42.0));
}

void test_rms_module_regression::injectSymmetricValues()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    TestDspInterfacePtr rmsDspInterface = testRunner.getDspInterface(rmsEntityId);

    RMSMODULE::cRmsModule *rmsModule = static_cast<RMSMODULE::cRmsModule*>(testRunner.getModule("rmsmodule", rmsEntityId));
    RMSMODULE::cRmsModuleConfiguration config;
    config.setConfiguration(rmsModule->getConfiguration());

    DemoValuesDspRms demoDspValue(config.getConfigurationData()->m_valueChannelList);
    demoDspValue.setAllValuesSymmetric(230, 5);
    rmsDspInterface->fireActValInterrupt(demoDspValue.getDspValues(), 0 /* dummy */);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpSymmetric.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << rmsEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_rms_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-rms-moduleconfig-from-resource.json");
    TestDspInterfacePtr rmsDspInterface = testRunner.getDspInterface(rmsEntityId);

    QString measProgramDumped = TestLogHelpers::dump(rmsDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}
