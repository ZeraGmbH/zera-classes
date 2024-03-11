#include "test_adj_module_gain.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "demovaluesdspadjustment.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_gain)

constexpr double testvoltage = 100;
constexpr double testcurrent = 10;
constexpr double testangle = 0;
constexpr double testfrequency = 50;
constexpr double limitOffset = 0.1;

void test_adj_module_gain::noActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    ScpiModuleClientBlocked scpiClient;
    // Init is missing by intention
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,199.9893000801;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::validActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::withinLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::withinLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::setActualTestValues(ModuleManagerTestRunner &testRunner)
{
    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspAdjustment dspValues(dspInterfaces[DSP_INTERFACE_DFT]->getValueList());
    dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireActualValues(
        dspInterfaces[DSP_INTERFACE_DFT],
        dspInterfaces[DSP_INTERFACE_FFT],
        dspInterfaces[DSP_INTERFACE_RANGE_PROGRAM], // Range is for frequency only
        dspInterfaces[DSP_INTERFACE_RMS]);
}
