#include "test_dspsuper_global_integration_time.h"
#include "dspsupermoduleintegrationcomponentfinder.h"
#include "modulemanagertestrunner.h"
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_dspsuper_global_integration_time)

void test_dspsuper_global_integration_time::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
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

static int constexpr entityIdDspSuper = 9000;
static int constexpr entityIdTime1st = 1040;
static int constexpr entityIdTime2nd = 1050;
static int constexpr entityIdPeriod1st = 1070;
static int constexpr entityIdPeriod2nd = 1071;

void test_dspsuper_global_integration_time::integrationComponentsFindTwoTimeTwoPeriod()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();

    QList<DspSuperModuleIntegrationComponentFinder::Component> timeEntityComponents =
        DspSuperModuleIntegrationComponentFinder::findIntegrationTimeComponents(veinStorageDb);
    QCOMPARE(timeEntityComponents.count(), 2);
    QCOMPARE(timeEntityComponents[0].entityId, entityIdTime1st);
    QCOMPARE(timeEntityComponents[0].componentName, "PAR_Interval");
    QCOMPARE(timeEntityComponents[1].entityId, entityIdTime2nd);
    QCOMPARE(timeEntityComponents[1].componentName, "PAR_Interval");
    QList<DspSuperModuleIntegrationComponentFinder::Component> periodEntityComponents =
        DspSuperModuleIntegrationComponentFinder::findIntegrationPeriodComponents(veinStorageDb);
    QCOMPARE(periodEntityComponents.count(), 2);
    QCOMPARE(periodEntityComponents[0].entityId, entityIdPeriod1st);
    QCOMPARE(periodEntityComponents[0].componentName, "PAR_Interval");
    QCOMPARE(periodEntityComponents[1].entityId, entityIdPeriod2nd);
    QCOMPARE(periodEntityComponents[1].componentName, "PAR_Interval");
}

void test_dspsuper_global_integration_time::globalIntegrationTimeComponentFollowsFirst()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);

    testRunner.setVfComponent(entityIdTime1st, "PAR_Interval", 10);
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 10);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 10);

    // check no loop/cross change
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);
}

void test_dspsuper_global_integration_time::globalIntegrationTimeComponentNotFollowsSecond()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);

    testRunner.setVfComponent(entityIdTime2nd, "PAR_Interval", 20);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 20);

    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
}

void test_dspsuper_global_integration_time::globalIntegrationPeriodComponentFollowsFirst()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);

    testRunner.setVfComponent(entityIdPeriod1st, "PAR_Interval", 101);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 101);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 101);

    // check no loop/cross change
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);
}

void test_dspsuper_global_integration_time::globalIntegrationPeriodComponentNotFollowsSecond()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);

    testRunner.setVfComponent(entityIdPeriod2nd, "PAR_Interval", 20);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 20);

    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
}

void test_dspsuper_global_integration_time::componentsTimeIntegrationFollowGlobal()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 1);

    testRunner.setVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime", 10);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalTime"), 10);
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 10);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 10);

    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
}

void test_dspsuper_global_integration_time::componentsPeriodIntegrationFollowGlobal()
{
    ModuleManagerTestRunner testRunner(":/sessions/two-time-two-period-integration.json");
    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 200);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 200);

    testRunner.setVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod", 201);
    QCOMPARE(testRunner.getVfComponent(entityIdDspSuper, "PAR_IntervalGlobalPeriod"), 201);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod1st, "PAR_Interval"), 201);
    QCOMPARE(testRunner.getVfComponent(entityIdPeriod2nd, "PAR_Interval"), 201);

    QCOMPARE(testRunner.getVfComponent(entityIdTime1st, "PAR_Interval"), 1);
    QCOMPARE(testRunner.getVfComponent(entityIdTime2nd, "PAR_Interval"), 1);
}
