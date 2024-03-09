#include "test_adj_module_phase.h"
#include "adjustmentmodule.h"
#include "demovaluesdspadjustment.h"
#include "modulemanagertestrunner.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_phase)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;
constexpr double testangle = 0;
constexpr double testfrequency = 50;

void test_adj_module_phase::noActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::validActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspAdjustment dspValues(dspInterfaces[DSP_INTERFACE_DFT]->getValueList());
    dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireActualValues(
        dspInterfaces[DSP_INTERFACE_DFT],
        dspInterfaces[DSP_INTERFACE_FFT],
        dspInterfaces[DSP_INTERFACE_RANGE_PROGRAM], // Range is for frequency only
        dspInterfaces[DSP_INTERFACE_RMS]);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspAdjustment dspValues(dspInterfaces[DSP_INTERFACE_DFT]->getValueList());
    dspValues.setAllValuesSymmetric(testvoltage, testcurrent, testangle, testfrequency);
    dspValues.fireActualValues(
        dspInterfaces[DSP_INTERFACE_DFT],
        dspInterfaces[DSP_INTERFACE_FFT],
        dspInterfaces[DSP_INTERFACE_RANGE_PROGRAM], // Range is for frequency only
        dspInterfaces[DSP_INTERFACE_RMS]);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+4");
}

