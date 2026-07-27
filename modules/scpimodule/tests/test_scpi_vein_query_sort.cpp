#include "test_scpi_vein_query_sort.h"
#include "scpimodule.h"
#include "scpimodulenetclientblocked.h"
#include "scpitestclient.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_scpi_vein_query_sort)

void test_scpi_vein_query_sort::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");

    // avoid overload on IAUX hand have clamps
    QList<AbstractMockAllServices::clampParam> clampParams;
    clampParams.append({"IAUX", cClamp::CL120A});
    m_testRunner->addClamps(clampParams);

    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);
    client.sendReceiveNotSorted("SENSE:RNG1:IAUX:RANGE C100A;", false);

    m_testRunner->fireActualValues();
    TimeMachineObject::feedEventLoop();
}

void test_scpi_vein_query_sort::defaultEnabled()
{
    // Vein
    QCOMPARE(m_testRunner->getVfComponent(9999, "PAR_QueryResponseSortActive"), QVariant(true));
    // Module
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    QCOMPARE(scpiModule->getSortQueryResponse(), true);

    // send queries that do sort
    SCPIMODULE::ScpiTestClient clientTest(scpiModule);
    clientTest.sendScpiCmds("MEASURE:DFT1:UL1?\n*OPC?");
    ScpiModuleNetClientBlocked clientEtherProduction;
    clientEtherProduction.sendMulti(QByteArrayList() << "MEASURE:DFT1:UL1?" << "*OPC?");
    TimeMachineObject::feedEventLoop();

    m_testRunner->fireActualValues();
    TimeMachineObject::feedEventLoop();

    // first check: are results different not sorted / sorted
    NullableStringList notSortedResponses = clientTest.getResponsesNotSorted();
    QCOMPARE(notSortedResponses[0].getStr(), "+1");
    QCOMPARE(notSortedResponses[1].getStr(), "DFT1:UL1:[V]:325.26913,0;");
    NullableStringList sortedResponses = clientTest.getResponsesSorted();
    QCOMPARE(sortedResponses[0].getStr(), "DFT1:UL1:[V]:325.26913,0;");
    QCOMPARE(sortedResponses[1].getStr(), "+1");

    QByteArrayList productionResponse = clientEtherProduction.receiveMulti();
    QCOMPARE(productionResponse[0], sortedResponses[0].getStr().toLocal8Bit());
    QCOMPARE(productionResponse[1], sortedResponses[1].getStr().toLocal8Bit());
}

void test_scpi_vein_query_sort::switchOff()
{
    m_testRunner->setVfComponent(9999, "PAR_QueryResponseSortActive", false);

    // Vein
    QCOMPARE(m_testRunner->getVfComponent(9999, "PAR_QueryResponseSortActive"), QVariant(false));
    // Module
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    QCOMPARE(scpiModule->getSortQueryResponse(), false);

    // send queries that do sort
    SCPIMODULE::ScpiTestClient clientTest(scpiModule);
    clientTest.sendScpiCmds("MEASURE:DFT1:UL1?\n*OPC?");
    ScpiModuleNetClientBlocked clientEtherProduction;
    clientEtherProduction.sendMulti(QByteArrayList() << "MEASURE:DFT1:UL1?" << "*OPC?");
    TimeMachineObject::feedEventLoop();

    m_testRunner->fireActualValues();
    TimeMachineObject::feedEventLoop();

    // first check: are results different not sorted / sorted
    NullableStringList notSortedResponses = clientTest.getResponsesNotSorted();
    QCOMPARE(notSortedResponses[0].getStr(), "+1");
    QCOMPARE(notSortedResponses[1].getStr(), "DFT1:UL1:[V]:325.26913,0;");
    NullableStringList sortedResponses = clientTest.getResponsesSorted();
    QCOMPARE(sortedResponses[0].getStr(), "DFT1:UL1:[V]:325.26913,0;");
    QCOMPARE(sortedResponses[1].getStr(), "+1");

    QByteArrayList productionResponse = clientEtherProduction.receiveMulti();
    QCOMPARE(productionResponse[0], notSortedResponses[0].getStr().toLocal8Bit());
    QCOMPARE(productionResponse[1], notSortedResponses[1].getStr().toLocal8Bit());
}
