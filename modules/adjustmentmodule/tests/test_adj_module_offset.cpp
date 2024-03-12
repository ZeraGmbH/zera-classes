#include "test_adj_module_offset.h"
#include "adjustmentmodule.h"
#include "demovaluesdspadjustment.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_offset)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;
constexpr double testangle = 0;
constexpr double testfrequency = 50;

void test_adj_module_offset::validActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_offset::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyNonOffsetChannel()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UL1,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,8V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::setActualTestValues(ModuleManagerTestRunner &testRunner)
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
