#include "test_adj_module_phase.h"
#include "adjustmentmodule.h"
#include "adjustmentmodulemeasprogram.h"
#include "demovaluesdspadjustment.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_phase)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;
constexpr double testangle = 0;
constexpr double testfrequency = 50;
constexpr double limitOffset = 0.1;

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
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absOutOfRangeLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,-0.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absOutOfRangeUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,360.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitLowerIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefAngle = adjustAngle(-maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    //QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefAngle = adjustAngle(+maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    //QCOMPARE(response, "+4");
}

void test_adj_module_phase::withinLimitLowerIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefAngle = adjustAngle(-maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::withinLimitUpperIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    setActualTestValues(testRunner);

    double adjRefAngle = adjustAngle(+maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::setActualTestValues(ModuleManagerTestRunner &testRunner)
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

double test_adj_module_phase::adjustAngle(double angle)
{
    if(angle < 0)
        angle += 360;
    if(angle > 360)
        angle -= 360;
    return angle;
}
