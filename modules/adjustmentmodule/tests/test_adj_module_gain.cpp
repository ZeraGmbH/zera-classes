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
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:ampl UL1,250V,100;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::outOfLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}

void test_adj_module_gain::withinLimitLower()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    double adjRefVoltage = testvoltage * (1-maxAmplitudeErrorPercent/100) + limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::withinLimitUpper()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    double adjRefVoltage = testvoltage * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl UL1,250V,%1;|*stb?").arg(adjRefVoltage).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+0");
}

void test_adj_module_gain::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);
    AdjModuleTestHelper::setActualTestValues(testRunner, testvoltage, testcurrent, testangle, testfrequency);

    double adjRefCurrent = testcurrent * (1+maxAmplitudeErrorPercent/100) - limitOffset;
    QByteArray send = QString("calc:adj1:ampl IL1,2.5A,%1;|*stb?").arg(adjRefCurrent).toLatin1();
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive(send);
    QCOMPARE(response, "+4");
}
