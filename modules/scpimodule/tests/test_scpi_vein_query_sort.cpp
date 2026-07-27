#include "test_scpi_vein_query_sort.h"
#include "scpimodule.h"
#include <timerfactoryqtfortest.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_vein_query_sort)

void test_scpi_vein_query_sort::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
}

void test_scpi_vein_query_sort::defaultEnabled()
{
    QCOMPARE(m_testRunner->getVfComponent(9999, "PAR_QueryResponseSortActive"), QVariant(true));
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    QCOMPARE(scpiModule->getSortQueryResponse(), true);
}

void test_scpi_vein_query_sort::switchOff()
{
    m_testRunner->setVfComponent(9999, "PAR_QueryResponseSortActive", false);

    QCOMPARE(m_testRunner->getVfComponent(9999, "PAR_QueryResponseSortActive"), QVariant(false));
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    QCOMPARE(scpiModule->getSortQueryResponse(), false);
}
