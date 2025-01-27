#include "test_adj_module_gain.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
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
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitLowerInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitUpperInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefCurrent = testcurrent * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl IL1,2.5A,%1;|*stb?").arg(adjRefCurrent).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::oneNodeWithinLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain::oneNodeWithinLimitLowerInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    // node load point is as reference says
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain::oneNodeWithinLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain::oneNodeWithinLimitUpperInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain::twoNodesCheckNodesGenerated()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));
}

void test_adj_module_gain::twoNodesCheckNodesGeneratedInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, -2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, -adjRefVoltage2/(2*testvoltage));

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, -adjRefVoltage2/(2*testvoltage));

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, -adjRefVoltage2/(2*testvoltage));
}

void test_adj_module_gain::oneNodeCheckCorrectionsCalculated()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::oneNodeCheckCorrectionsCalculatedInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, -2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedLinear()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedLinearInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, -2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedNonLinear()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    double error = 1.01;
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(testvoltage*error).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(2*testvoltage*error*error).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    // let's consider this a regression test and trust coefficients for now
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 1.0001);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 9.80392e-05);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedNonLinearInverted()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    ScpiModuleClientBlocked scpiClient;

    double error = 1.01;
    // we measure negative on inverted
    AdjModuleTestHelper::setActualTestValues(testRunner, -testvoltage, testcurrent, testangle, testfrequency);
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(testvoltage*error).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setActualTestValues(testRunner, -2*testvoltage, testcurrent, testangle, testfrequency);
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(2*testvoltage*error*error).toLatin1();
    response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    // let's consider this a regression test and trust coefficients for now
    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, -1.0001);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, -9.80392e-05);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}
