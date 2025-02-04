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
    DemoValuesDspRange rangeValues(rangeChannelCount);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 4); //UL1..3 = 4V, UAUX = 4V, IL1..3 = 4A, IAUX = 4A
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "5A");


    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 0); //UL1..3 = 0V, UAUX = 0V, IL1..3 = 0A, IAUX = 0A

    //After setting new range above (8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "100mV");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "25mA");


    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 15); //UL1..3 = 15V, UAUX = 15V, IL1..3 = 15A, IAUX = 15A

    //After setting new range above (100mV, 25mA), all range related processing is disabled for 1 Actual value interrupt cycle
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "10A");
}

void test_range_automatic::enableAndDisableRangeAutomatic()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspRange rangeValues(rangeChannelCount);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 0); //UL1..3 = 0V, UAUX = 0V, IL1..3 = 0A, IAUX = 0A
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

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
    DemoValuesDspRange rangeValues(rangeChannelCount);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 4); //UL1..3 = 4V, UAUX = 4V, IL1..3 = 4A, IAUX = 4A
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    testRunner.setVfComponent(rangeEntityId, RangeAutomaticComponent, 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "5A");

    //Introduce overload condition
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 15); //UL1..3 = 15V, UAUX = 15V, IL1..3 = 15A, IAUX = 15A

    //After setting new range above (8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 0);

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 1);
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "250V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "10A");


    //Remove overload condition
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsValue(i, 0.5); //UL1..3 = 0.5V, UAUX = 0.5V, IL1..3 = 0.5A, IAUX = 0.5A

    //After setting new range above (8V, 5A), all range related processing is disabled for 1 Actual value interrupt cycle
    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, "PAR_Overload"), 1);

    dspInterfaces[dspInterfaces::RangeModuleMeasProgram]->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, UL1RangeComponent), "8V");
    QCOMPARE(testRunner.getVfComponent(rangeEntityId, IL1RangeComponent), "500mA");
}

