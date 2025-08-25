#include "test_dosage.h"
#include "modulemanagertestrunner.h"
#include "demovaluesdspfft.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>


QTEST_MAIN(test_dosage)
static int constexpr rmsEntityId = 1040;
static int constexpr fftModule1EntityId = 1060;
static int constexpr power1module1EntityId = 1070;
static int constexpr power1module4EntityId = 1073;
static int constexpr dosageEntityId = 1600;

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int currentPhaseCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int rmsResultCount = voltagePhaseNeutralCount + currentPhaseCount + voltagePhasePhaseCount;
static constexpr int fftResultCount = 8;


void test_dosage::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_dosage::testDosageMaxAllBelowLimits()
{
    ModuleManagerTestRunner testRunner(":/def-session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module1EntityId);

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
    ModuleManagerTestRunner testRunner(":/def-session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module1EntityId);

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
    ModuleManagerTestRunner testRunner(":/def-session-maximal.json");
    TestDspInterfacePtr dspRmsInterface = testRunner.getDspInterface(rmsEntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module1EntityId);

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
    ModuleManagerTestRunner testRunner(":/def-session-minimal.json");
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module1EntityId);

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
    ModuleManagerTestRunner testRunner(":/def-session-minimal.json");
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module1EntityId);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(4);
    powActValues.append(3);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}

void test_dosage::testDosageDcAllBelowLimits()
{
    ModuleManagerTestRunner testRunner(":/dc-session.json");
    TestDspInterfacePtr dspFftInterface = testRunner.getDspInterface(fftModule1EntityId);
    TestDspInterfacePtr dspPowInterface = testRunner.getDspInterface(power1module4EntityId);

    DemoValuesDspFft dspValues(dspFftInterface->getValueList().count());
    dspValues.setValue(0, 0, 2, 0);     // int channelNo, int harmonicNo, float re, float im
    dspValues.setValue(1, 0, 2, 0);
    dspValues.setValue(2, 0, 2, 0);
    dspValues.setValue(3, 0, 2, 0);
    dspValues.setValue(4, 0, 2, 0);
    dspValues.setValue(5, 0, 2, 0);
    dspValues.setValue(6, 0, 2, 0);
    dspValues.setValue(7, 0, 1.9, 0);
    dspFftInterface->fireActValInterrupt(dspValues.getDspValues(), 0 /* dummy */);

    QVector<float> powActValues;
    for(int i = 0; i < MeasPhaseCount; i++)
        powActValues.append(0.9);
    powActValues.append(0.9);
    dspPowInterface->fireActValInterrupt(powActValues, 0 /* dummy */);

    TimeMachineForTest::getInstance()->processTimers(1000);
    QVERIFY(!testRunner.getVfComponent(dosageEntityId, "ACT_PowerAboveLimit").toBool());
}



