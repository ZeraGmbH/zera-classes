#include "test_scpi_all_responses.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <xmldocument.h>
#include <xmlelemiterstrategysort.h>
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <scpinode.h>
#include <testloghelpers.h>
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
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QFETCH(QString, scpiQuery);
    if (scpiQuery.startsWith("MEASURE") || scpiQuery.startsWith("READ"))
        m_testRunner->fireActualValues();
    client.sendScpiCmds(scpiQuery);
    TimeMachineObject::feedEventLoop();

    qInfo("Response: %s", qPrintable(client.getResponsesNotSorted()[0].getStr()));
    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
    // Transparent send to service (without command) is a super nasty/special exception - let's make it a test case
    bool nullExpected = scpiQuery == "CALCULATE:ADJ1:SEND?";
    QCOMPARE(client.getResponsesNotSorted()[0].isNull(), nullExpected);
}

void test_scpi_all_responses::checkScpiQueryEmptyResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    client.sendScpiCmds("STATUS:DEV1:CPUINFO?"); // We can rely this reponds empty on all non-target environments - until...
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
    QCOMPARE(client.getResponsesNotSorted()[0].isNull(), false);
    QCOMPARE(client.getResponsesNotSorted()[0].getStr(), "");
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
                addTestRow(queryTransaction);
                queryTransaction.clear();
            }
        }
    }
    if (!queryTransaction.isEmpty())
        addTestRow(queryTransaction);
}

void test_scpi_all_responses::checkScpiMulipleTransactionQueryResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QFETCH(QString, scpiQuery);
    if (scpiQuery.contains("MEASURE") || scpiQuery.contains("READ"))
        m_testRunner->fireActualValues();

    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiResponseNotSorted, this, [](const QString &scpiResponse) {
        qInfo("Response: %s", qPrintable(scpiResponse));
    });
    client.sendScpiCmds(scpiQuery);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
}

void test_scpi_all_responses::checkDumpAllQueriesInOneTransaction()
{
    restartServerForReproducabilityWithActualValues();
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    const QStringList fullList = getAllScpiQueriesFromDevIface();
    QStringList scpiQueries;
    for (const QString &scpiQuery : fullList) {
        if (ignoreForUnreproducableXmlOrFurtherInvestigation(scpiQuery))
            continue;
        scpiQueries.append(scpiQuery);
    }

    QSet<QString> scpiQueriesUniques;
    for (const QString &scpiQuery : scpiQueries)
        scpiQueriesUniques.insert(scpiQuery);

    QCOMPARE(scpiQueriesUniques.size(), scpiQueries.size()); // Check no double entries in scpiQueries
    QSet<QString> scpiQueriesUniquesSorted = scpiQueriesUniques;

    // Not sorted collector
    QStringList responses;
    QStringList scpiReceived;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiResponseNotSorted, this, [&](const QString &scpiResponse, bool isNull, const QString &scpi) {
        Q_UNUSED(isNull)
        responses.append(scpi + ":\n" + scpiResponse + "\n");
        scpiQueriesUniques.remove(scpi);
        scpiReceived.append(scpi);
    });

    // Sorted collector
    QStringList responsesSorted;
    QStringList scpiReceivedSorted;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiResponseSorted, this, [&](const QString &scpiResponse, bool isNull, const QString &scpi) {
        Q_UNUSED(isNull)
        responsesSorted.append(scpi + ":\n" + scpiResponse + "\n");
        scpiQueriesUniquesSorted.remove(scpi);
        scpiReceivedSorted.append(scpi);
    });

    client.sendScpiCmds(scpiQueries.join("\n"));
    TimeMachineObject::feedEventLoop();
    m_testRunner->fireActualValues();
    TimeMachineObject::feedEventLoop();

    const int queryCount = scpiQueries.size();
    QCOMPARE(client.getAllHandledResponseCount(), queryCount);
    QCOMPARE(client.getUnhandledResponses(), 0);

    // Not sorted
    QVERIFY(TestLogHelpers::compareAndLogOnDiffFile(":/scpi-dumps/dumped-all-queries-in-one-transaction", responses.join("\n")));
    QCOMPARE(responses.size(), queryCount);
    QCOMPARE(scpiQueriesUniques.count(), 0); // this is the silver bullet to find out transactions are not messed up!!!
    QVERIFY(scpiReceived != scpiQueries); // make sure sorter really sorts

    // Sorted
    QVERIFY(TestLogHelpers::compareAndLogOnDiffFile(":/scpi-dumps/dumped-all-queries-in-one-transaction-sorted", responsesSorted.join("\n")));
    QCOMPARE(responsesSorted.size(), queryCount);
    QCOMPARE(scpiQueriesUniquesSorted.count(), 0);
    QCOMPARE(scpiReceivedSorted, scpiQueries);
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
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QFETCH(QString, scpiCommand);
    qInfo("%s", qPrintable(scpiCommand));

    QCOMPARE(client.getAtLeastOneResponse(), false);
    client.sendScpiCmds(scpiCommand);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getResponsesNotSorted()[0].isNull(), true);
    QCOMPARE(client.getResponsesNotSorted()[0].getStr(), "");
}

void test_scpi_all_responses::checkScpiMulipleTransactionCmdResponse_data()
{
    QTest::addColumn<QString>("scpiCommand");

    constexpr int transactionSize = 6; // Played around with this to get mix of e.g CONFIGURATION & SENSE
    const QStringList scpiCommands = getAllScpiCommandsWithParamFromDevIface();
    QStringList cmdTransaction;
    for (const QString &scpiCommand : scpiCommands) {
        bool skipHere = ignoreToSpeedup(scpiCommand) ||
                        scpiCommand.startsWith("CONFIGURATION:SYST:NAMESESSION") ||
                        scpiCommand.startsWith("CONFIGURATION:SYST:XSESSION");
        if (!skipHere) {
            cmdTransaction.append(scpiCommand);
            if (cmdTransaction.size() >= transactionSize) {
                addTestRow(cmdTransaction);
                cmdTransaction.clear();
            }
        }
    }
    if (!cmdTransaction.isEmpty())
        addTestRow(cmdTransaction);
}

void test_scpi_all_responses::checkScpiMulipleTransactionCmdResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QFETCH(QString, scpiCommand);
    qInfo("%s", qPrintable(scpiCommand));

    QCOMPARE(client.getAtLeastOneResponse(), false);
    client.sendScpiCmds(scpiCommand);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getResponsesNotSorted()[0].isNull(), true);
    QCOMPARE(client.getResponsesNotSorted()[0].getStr(), "");
}

void test_scpi_all_responses::restartServerForReproducabilityWithActualValues()
{
    m_testRunner.reset();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
    m_testRunner->fireActualValues();
    TimeMachineObject::feedEventLoop();
}

QStringList test_scpi_all_responses::getAllScpiQueriesFromDevIface()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getResponsesNotSorted()[0].getStr();

    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategySort>());
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
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getResponsesNotSorted()[0].getStr();

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

bool test_scpi_all_responses::ignoreForUnreproducableXmlOrFurtherInvestigation(const QString &scpiPath)
{
    return
        scpiPath == "DEVICE:IFACE?" ||
        scpiPath.startsWith("CALCULATE:ADJ1"); // This
}

QString test_scpi_all_responses::scpiShortHeader(const QString scpiCmd)
{
    QString testName = scpiCmd.split(" ")[0]; // remove param
    testName.remove(";");
    testName.remove("?");
    const QStringList scpiEntries = testName.split(":");
    QStringList scpiEntriesShort;
    for (const QString &scpiHeader : scpiEntries)
        scpiEntriesShort.append(ScpiNode::createShortHeader(scpiHeader));
    return scpiEntriesShort.join(":");
}

void test_scpi_all_responses::addTestRow(const QStringList scpiTransaction)
{
    QStringList shortTestNamesNoParams;
    for (const QString &cmd : scpiTransaction)
        shortTestNamesNoParams.append(scpiShortHeader(cmd));
    QTest::newRow(shortTestNamesNoParams.join(" ").toLatin1()) << scpiTransaction.join("\n");
}
