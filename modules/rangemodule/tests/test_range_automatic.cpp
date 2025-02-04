#include "test_range_automatic.h"
#include "demovaluesdsprange.h"
#include "modulemanagertestrunner.h"
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_range_automatic)

static int constexpr rangeEntityId = 1020;
static QString UL1RangeComponent("PAR_Channel1Range");
static QString IL1RangeComponent("PAR_Channel4Range");
static QString RangeAutomaticComponent("PAR_RangeAutomatic");

void test_range_automatic::defaultRangesAndSetting()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, RangeAutomaticComponent), 0);
}

enum dspInterfaces{
    RangeObsermatic,
    AdjustManagement,
    RangeModuleMeasProgram
};

static constexpr int rangeChannelCount = 8;

void test_range_automatic::testRangeAutomatic()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 4);
    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 0);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle.
    //So, fire an extra interrupt.
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 15);
    //After setting new range (above 100mV, 25mA), all range related processing is disabled for 1 Actual value interrupt cycle.
    //So, fire an extra interrupt.
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 15);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
}

void test_range_automatic::enableAndDisableRangeAutomatic()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 0);

    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");

    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "100mV"); //unchanged
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "25mA"); //unchanged
}

void test_range_automatic::softOverloadWithRangeAutomatic()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 4);
    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    //Introduce overload condition
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 15);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 0);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 15);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "10A");

    //Remove overload condition
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 0.5);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    //After setting new range (above 8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    //So, fire an extra interrupt.
    fireNewRmsValues(dspInterfaces[dspInterfaces::RangeModuleMeasProgram], 0.5);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "500mA");
}

void test_range_automatic::fireNewRmsValues(const TestDspInterfacePtr &dspInterface, float rmsValue)
{
    DemoValuesDspRange rangeValues(rangeChannelCount);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, rmsValue);
    dspInterface->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}
