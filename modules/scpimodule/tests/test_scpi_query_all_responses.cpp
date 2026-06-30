#include "test_scpi_query_all_responses.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <xmldocument.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_query_all_responses)

void test_scpi_query_all_responses::initTestCase()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
}

void test_scpi_query_all_responses::checkScpiQueryResponse_data()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getLastResponse();

    QTest::addColumn<QString>("scpiPath");
    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategyTree>());
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString type = element.attribute("Type");
        const QString scpiPath = element.attribute("ScpiPath");
        if (!scpiPath.isEmpty() && !type.isEmpty() && type.contains("Query"))
            QTest::newRow(scpiPath.toLatin1()) << scpiPath;
        iter.next();
    }
}

void test_scpi_query_all_responses::checkScpiQueryResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiPath);
    if (scpiPath.startsWith("MEAS") || scpiPath.startsWith("READ")) {
        m_testRunner->fireActualValues();
    }
    client.sendScpiCmds(scpiPath + "?");
    TimeMachineObject::feedEventLoop();

    qInfo("Response: %s", qPrintable(client.getLastResponse()));
    QCOMPARE(client.getUnhandledResponses(), 0);
}
