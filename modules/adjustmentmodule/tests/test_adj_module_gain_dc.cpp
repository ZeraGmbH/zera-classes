#include "test_adj_module_gain_dc.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_adj_module_gain_dc)

static constexpr double testvoltage = 100;
static constexpr double testcurrent = 10;
static constexpr double limitOffset = 0.1;
static const char* calcCmd = "calc:adj1:dcam ";

void test_adj_module_gain_dc::init()
{
    if(!m_testRunner) {
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-minimal-dc.json", true);
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    }
    QString response = m_scpiClient->sendReceive("*cls|CALC:ADJ1:INIT UAUX,250V;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain_dc::destroyCommonTestRunner()
{
    if(m_scpiClient) {
        m_scpiClient = nullptr;
        m_testRunner = nullptr;
    }
}

void test_adj_module_gain_dc::noActValuesWithPermission()
{
    QString response = m_scpiClient->sendReceive(QByteArray(calcCmd) + "UAUX,250V,199.9893000801;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::validActValuesWithPermission()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QString response = m_scpiClient->sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain_dc::validActValuesWithPermissionNeg()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QString response = m_scpiClient->sendReceive(QByteArray(calcCmd) + "UAUX,250V,-100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain_dc::validActValuesWithoutPermission()
{
    destroyCommonTestRunner();
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", false);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);
    QString response = scpiClient.sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLower()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLowerNeg1()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitLowerNeg2()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpper()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpperNeg1()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::outOfLimitUpperNeg2()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::denyRangeNotSet()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,8V,%1;|*stb?").arg(testvoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::withinLimitLowerButAcValues()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, 0, 50);
    QString response = m_scpiClient->sendReceive(QByteArray(calcCmd) + "UAUX,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain_dc::oneNodeWithinLimitLower()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitLowerNeg1()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitLowerNeg2()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpper()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpperNeg1()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, -testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}

void test_adj_module_gain_dc::oneNodeWithinLimitUpperNeg2()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QByteArray(calcCmd) + QString("UAUX,250V,%1;|*stb?").arg(-adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M6:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, -adjRefVoltage);
    QCOMPARE(node.m_correction, -adjRefVoltage/testvoltage);
}
