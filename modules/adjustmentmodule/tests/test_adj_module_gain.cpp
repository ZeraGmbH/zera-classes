#include "test_adj_module_gain.h"
#include "adjustmentmodulemeasprogram.h"
#include "adjmoduletesthelper.h"
#include <QTest>

QTEST_MAIN(test_adj_module_gain)

constexpr double testvoltage = 100;
constexpr double testcurrent = 10;
constexpr double testangle = 0;
constexpr double testfrequency = 50;
constexpr double limitOffset = 0.1;

void test_adj_module_gain::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-minimal.json", true);
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    QString response = m_scpiClient->sendReceive("*cls|CALC:ADJ1:INIT UL1,250V;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::resetLooksLikeABugOnInit()
{
    destroyCommonTestRunner();
    init();
}

void test_adj_module_gain::destroyCommonTestRunner()
{
    if(m_scpiClient)
        m_scpiClient = nullptr;
    if(m_testRunner)
        m_testRunner = nullptr;
}

void test_adj_module_gain::noActValuesWithPermission()
{
    QString response = m_scpiClient->sendReceive("calc:adj1:ampl UL1,250V,199.9893000801;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::validActValuesWithPermission()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QString response = m_scpiClient->sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::validActValuesWithoutPermission()
{
    destroyCommonTestRunner();
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    ScpiModuleClientBlocked scpiClient;

    AdjModuleTestHelper::setAllValuesSymmetricAc(testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitLower()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitUpper()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::denyRangeNotSet()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QByteArray send = QString("calc:adj1:ampl UL1,8V,%1;|*stb?").arg(testvoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::withinLimitLowerButDcValues()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(testvoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::oneNodeWithinLimitLower()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain::oneNodeWithinLimitUpper()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);
}

void test_adj_module_gain::twoNodesCheckNodesGenerated()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:0?;");
    AdjModuleTestHelper::TAdjNodeValues node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage);
    QCOMPARE(node.m_correction, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:1?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:2?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:NODE:3?;");
    node = AdjModuleTestHelper::parseNode(response);
    QCOMPARE(node.m_loadPoint, adjRefVoltage2);
    QCOMPARE(node.m_correction, adjRefVoltage2/(2*testvoltage));
}

void test_adj_module_gain::oneNodeCheckCorrectionsCalculated()
{
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");
    response = m_scpiClient->sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedLinear()
{
    qWarning("Before reset 0: %s",
          qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;")));
    qWarning("Before reset 1: %s",
          qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;")));
    resetLooksLikeABugOnInit();
    qWarning("After reset 0: %s",
          qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;")));
    qWarning("After reset 1: %s",
          qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;")));

    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    double adjRefVoltage2 = 2*adjRefVoltage;
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage2).toLatin1();
    response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, adjRefVoltage/testvoltage);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}

void test_adj_module_gain::twoNodesCheckCorrectionsCalculatedNonLinear()
{
    qWarning("Before reset 0: %s",
             qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;")));
    qWarning("Before reset 1: %s",
             qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;")));
    resetLooksLikeABugOnInit();
    qWarning("After reset 0: %s",
             qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;")));
    qWarning("After reset 1: %s",
             qPrintable(m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;")));

    double error = 1.01;
    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, testvoltage, testcurrent, testangle, testfrequency);
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(testvoltage*error).toLatin1();
    QString response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    AdjModuleTestHelper::setAllValuesSymmetricAc(*m_testRunner, 2*testvoltage, testcurrent, testangle, testfrequency);
    send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(2*testvoltage*error*error).toLatin1();
    response = m_scpiClient->sendReceive(send);
    QCOMPARE(response, "+0");

    response = m_scpiClient->sendReceive("calc:adj1:comp 1;|*stb?");
    QCOMPARE(response, "+0");

    // let's consider this a regression test and trust coefficients for now
    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:0?;");
    double adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 1.0001);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:1?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 9.80392e-05);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:2?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);

    response = m_scpiClient->sendReceive("calc:adj1:send? 6307,SENSE:M0:250V:CORRECTION:GAIN:COEFFICIENT:3?;");
    adjCoefficient = response.toDouble();
    QCOMPARE(adjCoefficient, 0.0);
}
