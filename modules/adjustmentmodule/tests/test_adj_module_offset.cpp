#include "test_adj_module_offset.h"
#include "adjmoduletesthelper.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_offset)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;

void test_adj_module_offset::validActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_offset::validActValuesWithoutPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyNonOffsetChannel()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UL1,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyRangeNotSet()
{
    ModuleManagerTestRunner testRunner(":/session-minimal-dc.json", true);
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,8V,0;|*stb?");
    QCOMPARE(response, "+4");
}
