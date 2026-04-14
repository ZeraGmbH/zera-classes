#include "test_dspsuper_global_integration_time.h"
#include "dspsupermoduleintegrationcomponentfinder.h"
#include "modulemanagertestrunner.h"
#include <QTest>

QTEST_MAIN(test_dspsuper_global_integration_time)

//static int constexpr dspSuperEntityId = 9000;

void test_dspsuper_global_integration_time::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_dspsuper_global_integration_time::integrationComponentsFindNoneOnMinimal()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();

    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(veinStorageDb).count(), 0);
    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(veinStorageDb).count(), 0);
}

void test_dspsuper_global_integration_time::integrationComponentsFindTwoTime()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-integration.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();

    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(veinStorageDb).count(), 2);
    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(veinStorageDb).count(), 0);
}

void test_dspsuper_global_integration_time::integrationComponentsFindTwoPeriod()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-period-integration.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();

    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(veinStorageDb).count(), 0);
    QCOMPARE(DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(veinStorageDb).count(), 2);
}

void test_dspsuper_global_integration_time::integrationComponentsFindTwoTimeTwoPeriod()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();

    QList<DspSuperModuleIntegrationComponentFinder::Components> timeEntityComponents =
        DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(veinStorageDb);
    QCOMPARE(timeEntityComponents.count(), 2);
    QCOMPARE(timeEntityComponents[0].entityId, 1040);
    QCOMPARE(timeEntityComponents[0].componentName, "PAR_Interval");
    QCOMPARE(timeEntityComponents[1].entityId, 1050);
    QCOMPARE(timeEntityComponents[1].componentName, "PAR_Interval");
    QList<DspSuperModuleIntegrationComponentFinder::Components> periodEntityComponents =
        DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(veinStorageDb);
    QCOMPARE(periodEntityComponents.count(), 2);
    QCOMPARE(periodEntityComponents[0].entityId, 1070);
    QCOMPARE(periodEntityComponents[0].componentName, "PAR_Interval");
    QCOMPARE(periodEntityComponents[1].entityId, 1071);
    QCOMPARE(periodEntityComponents[1].componentName, "PAR_Interval");
}
