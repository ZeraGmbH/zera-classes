#include "test_scpi_all_responses.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <xmldocument.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_scpi_all_responses)

void test_scpi_all_responses::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
}

void test_scpi_all_responses::checkScpiQueryResponse_data()
{
    QTest::addColumn<QString>("scpiQuery");

    const QStringList scpiQueries = getAllScpiQueriesFromDevIface();
    for (const QString &scpiQuery : scpiQueries) {
        if (!ignoreToSpeedup(scpiQuery))
            QTest::newRow(scpiQuery.toLatin1()) << scpiQuery;
    }
}

void test_scpi_all_responses::checkScpiQueryResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiQuery);
    if (scpiQuery.startsWith("MEASURE") || scpiQuery.startsWith("READ"))
        m_testRunner->fireActualValues();
    client.sendScpiCmds(scpiQuery);
    TimeMachineObject::feedEventLoop();

    qInfo("Response: %s", qPrintable(client.getLastResponse().getStr()));
    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
    bool nullExpected = scpiQuery == "CALCULATE:ADJ1:SEND?"; // Transparent send (without command) is a super nasty/special exception - let's make it a test case
    QCOMPARE(client.getLastResponse().isNull(), nullExpected);
}

void test_scpi_all_responses::checkScpiQueryEmptyResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    client.sendScpiCmds("STATUS:DEV1:CPUINFO?"); // We can rely this reponds empty on all non-target environments - until...
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
    QCOMPARE(client.getLastResponse().isNull(), false);
    QCOMPARE(client.getLastResponse().getStr(), "");
}

void test_scpi_all_responses::checkScpiMulipleTransactionQueryResponse_data()
{
    QTest::addColumn<QString>("scpiQuery");

    constexpr int transactionSize = 4; // Played around with this to get mix of e.g CONFIGURATION & SENSE
    const QStringList scpiQueries = getAllScpiQueriesFromDevIface();
    QStringList queryTransaction;
    for (const QString &scpiQuery : scpiQueries) {
        if (!ignoreToSpeedup(scpiQuery)) {
            queryTransaction.append(scpiQuery);
            if (queryTransaction.size() >= transactionSize) {
                QTest::newRow(queryTransaction.join(" ").toLatin1()) << queryTransaction.join("\n");
                queryTransaction.clear();
            }
        }
    }
}

void test_scpi_all_responses::checkScpiMulipleTransactionQueryResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiQuery);
    if (scpiQuery.contains("MEASURE") || scpiQuery.contains("READ"))
        m_testRunner->fireActualValues();

    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, this, [](const QString &scpiResponse) {
        qInfo("Response: %s", qPrintable(scpiResponse));
    });
    client.sendScpiCmds(scpiQuery);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_all_responses::checkScpiCmdResponse_data()
{
    QTest::addColumn<QString>("scpiCommand");

    const QStringList scpiCommands = getAllScpiCommandsWithParamFromDevIface();
    for (const QString &scpiCommand : scpiCommands) {
        bool skipHere = ignoreToSpeedup(scpiCommand) ||
                        scpiCommand.startsWith("CONFIGURATION:SYST:NAMESESSION") ||
                        scpiCommand.startsWith("CONFIGURATION:SYST:XSESSION");
        if (!skipHere)
            QTest::newRow(scpiCommand.toLatin1()) << scpiCommand;
    }
}

void test_scpi_all_responses::checkScpiCmdResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiCommand);
    qInfo("%s", qPrintable(scpiCommand));

    QCOMPARE(client.getAtLeastOneResponse(), false);
    client.sendScpiCmds(scpiCommand);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getLastResponse().isNull(), true);
    QCOMPARE(client.getLastResponse().getStr(), "");
}

QStringList test_scpi_all_responses::getAllScpiQueriesFromDevIface()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getLastResponse().getStr();

    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategyTree>());
    QStringList scpiPaths;
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString type = element.attribute("Type");
        const QString scpiPath = element.attribute("ScpiPath");

        if (!scpiPath.isEmpty() && type.contains("Query"))
            scpiPaths.append(scpiPath + "?");
        iter.next();
    }
    return scpiPaths;
}

QStringList test_scpi_all_responses::getAllScpiCommandsWithParamFromDevIface()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getLastResponse().getStr();

    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategyTree>());
    QStringList scpiCommands;
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString type = element.attribute("Type");
        const QString scpiPath = element.attribute("ScpiPath");
        if (!scpiPath.isEmpty() && type.contains("Command")) {
            QString scpiParam;
            if (type.contains("Command+Par")) {
                const QString validPar = element.attribute("ValidPar");
                const QString max = element.attribute("Max");
                if (!validPar.isEmpty()) {
                    QStringList parameters = validPar.split(",");
                    if (parameters.size() > 0)
                        scpiParam = parameters[0];
                }
                else if(!max.isEmpty())
                    scpiParam = max;
            }
            QString scpiCmd;
            if(!scpiParam.isEmpty())
                scpiCmd = scpiPath + " " + scpiParam + ";";
            else
                scpiCmd = scpiPath + ";";
            scpiCommands.append(scpiCmd);
        }
        iter.next();
    }
    return scpiCommands;
}

bool test_scpi_all_responses::ignoreToSpeedup(const QString &scpiPath)
{
    const QStringList ignoreTags = QStringList()
                                   << "UL2" << ":U2"
                                   << "UL3" << ":U3"
                                   << "IL2" << "I2"
                                   << "IL3" << "I3"
                                   << "POW2" << "POW3"
                                   << ":P2" << ":P3"
                                   << ":Q2" << ":Q3"
                                   << ":S2" << ":S3"
                                   << ":OSC1:UAUX" << ":OSC1:IL1" << ":OSC1:IAUX"
                                   << ":BD12"
        ;
    for (const QString &ignoreTag : ignoreTags)
        if (scpiPath.contains(ignoreTag))
            return true;

    return false;
}
