#include "test_adj_module_gain_dc.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_gain_dc)

static constexpr double testvoltage = 100;
static constexpr double testcurrent = 10;
static constexpr double limitOffset = 0.1;
static const char* calcCmd = "calc:adj1:dcam ";

void test_adj_module_gain_dc::noActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    // Init is missing by intention
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,199.9893000801;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::validActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain_dc::validActValuesWithPermissionNeg()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,-100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain_dc::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", false);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLowerNeg1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLowerNeg2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpperNeg1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpperNeg2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,8V,%1;|*stb?").arg(testvoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::withinLimitLowerButAcValues()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricAc(testRunner, testvoltage, testcurrent, 0, 50);
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::oneNodeWithinLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitLowerNeg1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitLowerNeg2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpperNeg1()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpperNeg2()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = scpiClient.sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}
