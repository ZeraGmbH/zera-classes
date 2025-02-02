#include "test_adj_module_offset.h"
#include "adjmoduletesthelper.h"
#include <QTest>

QTEST_MAIN(test_adj_module_offset)

constexpr double testvoltage = 120;
constexpr double testcurrent = 10;

void test_adj_module_offset::init()
{
    if(!m_testRunner)
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-minimal-dc.json", true);
    if(!m_scpiClient)
        m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
    QString response = m_scpiClient->sendReceive("*cls|CALC:ADJ1:INIT UAUX,250V;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_offset::destroyCommonTestRunner()
{
    m_scpiClient = nullptr;
    m_testRunner = nullptr;
}

void test_adj_module_offset::validActValuesWithPermission()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QString response = m_scpiClient->sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+0");
}

void test_adj_module_offset::validActValuesWithoutPermission()
{
    destroyCommonTestRunner();
    ModuleManagerTestRunner testRunner(":/session-minimal.json", false);
    AdjModuleTestHelper::setAllValuesSymmetricDc(testRunner, testvoltage, testcurrent);

    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:offs UAUX,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyNonOffsetChannel()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QString response = m_scpiClient->sendReceive("calc:adj1:offs UL1,250V,0;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_offset::denyRangeNotSet()
{
    AdjModuleTestHelper::setAllValuesSymmetricDc(*m_testRunner, testvoltage, testcurrent);
    QString response = m_scpiClient->sendReceive("calc:adj1:offs UAUX,8V,0;|*stb?");
    QCOMPARE(response, "+4");
}
