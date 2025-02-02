#include "test_adj_module_phase.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
#include <QTest>

QTEST_MAIN(test_adj_module_phase)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;
constexpr double zeroangle = 0;
constexpr double testfrequency = 50;
constexpr double limitOffset = 0.1;

void test_adj_module_phase::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-minimal.json", true);
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    QString response = m_scpiClient->sendReceive("*cls|CALC:ADJ1:INIT IL1,10A;|*stb?");
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("*cls|CALC:ADJ1:INIT UL2,250V;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::destroyCommonTestRunner()
{
    if(m_scpiClient)
        m_scpiClient = nullptr;
    if(m_testRunner)
        m_testRunner = nullptr;
}

void test_adj_module_phase::noActValuesWithPermission()
{
    QString response = m_scpiClient->sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::validActValuesWithPermission()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::validActValuesWithoutPermission()
{
    destroyCommonTestRunner();
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = scpiClient.sendReceive("calc:adj1:phas IL1,10A,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absOutOfRangeLower()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:phas IL1,10A,-360.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absWithinRangeLower()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:phas IL1,10A,-360.0000;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::absOutOfRangeUpper()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:phas IL1,10A,360.0001;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::absWithinRangeUpper()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:phas IL1,10A,360.0000;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_phase::outOfLimitLowerIL1()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(zeroangle - maxPhaseErrorDegrees - limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitLowerIL1Neg()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngleWithNeg(zeroangle - maxPhaseErrorDegrees - limitOffset);
    QVERIFY(adjRefAngle < 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(zeroangle + maxPhaseErrorDegrees + limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1Neg()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = -360 + adjustAngle(zeroangle + maxPhaseErrorDegrees + limitOffset);
    QVERIFY(adjRefAngle < 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::oneNodeWithinLimitLowerIL1()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(zeroangle - maxPhaseErrorDegrees + limitOffset);
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeWithinLimitLowerIL1Neg()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = zeroangle - maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle < 0);
    // see oneNodeWithinLimitLowerIL1
    const double adjRefAngleNotNeg = adjustAngle(-maxPhaseErrorDegrees + limitOffset);
    QCOMPARE(cAdjustmentModuleMeasProgram::symAngle(adjRefAngle), cAdjustmentModuleMeasProgram::symAngle(adjRefAngleNotNeg));

    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeWithinLimitUpperIL1()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = zeroangle + maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeOnPointIL1()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = zeroangle;
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(zeroangle-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

constexpr double angle180 = 180.0; // 180 is special see cAdjustmentModuleMeasProgram::symAngle

void test_adj_module_phase::outOfLimitLowerIL1_180()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = angle180 - maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperIL1_180()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = angle180 + maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::oneNodewithinLimitLowerIL1_180()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = angle180 - maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodewithinLimitLowerIL1_180Neg()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = -angle180 - maxPhaseErrorDegrees + limitOffset;
    QVERIFY(adjRefAngle < 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodewithinLimitUpperIL1_180()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = angle180 + maxPhaseErrorDegrees - limitOffset;
    QVERIFY(adjRefAngle > 0);
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeOnPointIL1_180()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = angle180;
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::oneNodeOnPointIL1_180Neg()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, angle180, testfrequency);
    const double adjRefAngle = -angle180;
    QByteArray send = QString("calc:adj1:phas IL1,10A,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angle180-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M3:10A:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

constexpr double angleUL2 = 120;

void test_adj_module_phase::outOfLimitLowerUL2()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(angleUL2 + maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::outOfLimitUpperUL2()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::withinLimitLowerUL2()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(angleUL2 + maxPhaseErrorDegrees - limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angleUL2-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::withinLimitUpperUL2()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,250V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    double nodeValExpected = cAdjustmentModuleMeasProgram::symAngle(angleUL2-adjRefAngle);
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M1:250V:CORRECTION:PHASE:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, testfrequency);
    QCOMPARE(node.m_correction, nodeValExpected);
}

void test_adj_module_phase::denyRangeNotSet()
{
    AdjModuleTestHelper::setActualTestValues(*m_testRunner, testvoltage, testcurrent, zeroangle, testfrequency);
    const double adjRefAngle = adjustAngle(angleUL2 - maxPhaseErrorDegrees + limitOffset);
    QByteArray send = QString("calc:adj1:phas UL2,8V,%1;|*stb?").arg(adjRefAngle).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
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
