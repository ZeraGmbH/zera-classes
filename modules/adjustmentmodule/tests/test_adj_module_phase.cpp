#include "test_adj_module_phase.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
#include <cmath>
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_phase)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;
constexpr double zeroangle = 0;
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
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absOutOfRangeLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,-0.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absWithinRangeLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0.0000;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::absOutOfRangeUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,360.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absWithinRangeUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,360.0000;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::outOfLimitLowerIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(zeroangle - maxPhaseErrorDegrees - limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(zeroangle + maxPhaseErrorDegrees + limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::oneNodeWithinLimitLowerIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(zeroangle - maxPhaseErrorDegrees + limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeWithinLimitUpperIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = zeroangle + maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeOnPointIL1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = zeroangle;
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

constexpr double angle180 = 180.0; // 180 is special see cAdjustmentModuleMeasProgram::symAngle

void test_adj_module_phase::outOfLimitLowerIL1_180()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, angle180, testfrequency);

    const double adjRefAngle = angle180 - maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1_180()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, angle180, testfrequency);

    const double adjRefAngle = angle180 + maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

// Hmm we see some acurracy loss on 180°:
// Casting to float on QCOMPARE worked on X86-64 but failed on qemu-arm (OE) with
// |    Actual   (float(node.m_correction)): 5e-06
// |    Expected (float(nodeValExpected))  : 0
static constexpr double epsilon180 = 2e-5;

void test_adj_module_phase::oneNodewithinLimitLowerIL1_180()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, angle180, testfrequency);

    const double adjRefAngle = angle180 - maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    if(fabs(node.m_correction-nodeValExpected) > epsilon180) {
        qWarning("Too huge deviation: Expected %f, got %f", nodeValExpected, node.m_correction);
        QVERIFY(false);
    }
}

void test_adj_module_phase::oneNodewithinLimitUpperIL1_180()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, angle180, testfrequency);

    const double adjRefAngle = angle180 + maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    if(fabs(node.m_correction-nodeValExpected) > epsilon180) {
        qWarning("Too huge deviation: Expected %f, got %f", nodeValExpected, node.m_correction);
        QVERIFY(false);
    }
}

void test_adj_module_phase::oneNodeOnPointIL1_180()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, angle180, testfrequency);

    const double adjRefAngle = angle180;
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    if(fabs(node.m_correction-nodeValExpected) > epsilon180) {
        qWarning("Too huge deviation: Expected %f, got %f", nodeValExpected, node.m_correction);
        QVERIFY(false);
    }
}

constexpr double angleUL2 = 120;

void test_adj_module_phase::outOfLimitLowerUL2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(angleUL2 + maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperUL2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::withinLimitLowerUL2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(angleUL2 + maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angleUL2-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::withinLimitUpperUL2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angleUL2-adjRefAngle);
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);

    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,8V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

double test_adj_module_phase::adjustAngle(double angle)
{
    if(angle < 0)
        angle += 360;
    if(angle > 360)
        angle -= 360;
    return angle;
}

double test_adj_module_phase::adjustAngleWithNeg(double angle)
{
    if(angle < -360)
        angle += 360;
    if(angle > 360)
        angle -= 360;
    return angle;
}
