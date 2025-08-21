#include "test_dosage.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>


QTEST_MAIN(test_dosage)
static int constexpr rmsEntityId = 1040;
static int constexpr power1EntityId = 1070;
static int constexpr dosageEntityId = 1600;


static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int currentPhaseCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int rmsResultCount = voltagePhaseNeutralCount + currentPhaseCount + voltagePhasePhaseCount;
static constexpr int powMeasPhaseCount = 3;
static constexpr int powSumValueCount = 1;


void test_dosage::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_dosage::testDosageMaxAllBelowLimits()
{
    ModuleManagerTestRunner testRunner(":/session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1EntityId);

    QVector<float> rmsActValues;
    for(int i = 0; i < rmsResultCount; i++)
        rmsActValues.append(0.1);
    dspRmsInterface->fireActValInterrupt(rmsActValues, 0 /* dummy */);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(1.0);
    powActValues.append(1.0);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(!testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}

void test_dosage::testDosageMaxRmsUpperLimits()
{
    ModuleManagerTestRunner testRunner(":/session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1EntityId);

    QVector<float> rmsActValues;
    for(int i = 0; i < rmsResultCount; i++)
        rmsActValues.append(0.5);
    dspRmsInterface->fireActValInterrupt(rmsActValues, 0 /* dummy */);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(1);
    powActValues.append(3);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}

void test_dosage::testDosageMaxPowUpperLimits()
{
    ModuleManagerTestRunner testRunner(":/session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1EntityId);

    QVector<float> rmsActValues;
    for(int i = 0; i < rmsResultCount; i++)
        rmsActValues.append(0.3);
    dspRmsInterface->fireActValInterrupt(rmsActValues, 0 /* dummy */);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(4);
    powActValues.append(3);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}

void test_dosage::testDosageMinPowBelowLimits()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1EntityId);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(1.0);
    powActValues.append(1.0);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(!testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}

void test_dosage::testDosageMinPowUpperLimits()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1EntityId);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(4);
    powActValues.append(3);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}


