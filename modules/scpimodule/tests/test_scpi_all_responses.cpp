#include "test_scpi_all_responses.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <xmldocument.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_all_responses)

void test_scpi_all_responses::initTestCase()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
}

void test_scpi_all_responses::checkScpiQueryResponse_data()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getLastResponse().getStr();

    QTest::addColumn<QString>("scpiPath");

    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategyTree>());
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString type = element.attribute("Type");
        const QString scpiPath = element.attribute("ScpiPath");
        if (!ignoreToSpeedup(scpiPath) && !scpiPath.isEmpty() && !type.isEmpty() && type.contains("Query"))
            QTest::newRow(scpiPath.toLatin1()) << scpiPath;
        iter.next();
    }
}

void test_scpi_all_responses::checkScpiQueryResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiPath);
    if (scpiPath.startsWith("MEAS") || scpiPath.startsWith("READ")) {
        m_testRunner->fireActualValues();
    }
    client.sendScpiCmds(scpiPath + "?");
    TimeMachineObject::feedEventLoop();

    qInfo("Response: %s", qPrintable(client.getLastResponse().getStr()));
    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getUnhandledResponses(), 0);
    bool nullExpected = scpiPath == "CALCULATE:ADJ1:SEND"; // Transparent send (without command) is a super nasty/special exception - let's make it a test case
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

void test_scpi_all_responses::checkScpiCmdResponse_data()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("dev:iface?");
    TimeMachineObject::feedEventLoop();
    const QString &devIface = client.getLastResponse().getStr();

    QTest::addColumn<QString>("scpiPath");
    QTest::addColumn<QString>("scpiParam");

    XmlDocument xml;
    xml.loadXml(devIface, true);
    XmlElemIter iter = xml.root(std::make_unique<XmlElemIterStrategyTree>());
    while(!iter.isEnd()) {
        QDomElement element = iter.getElem();
        const QString type = element.attribute("Type");
        const QString scpiPath = element.attribute("ScpiPath");
        bool skipHere = ignoreToSpeedup(scpiPath) ||
                        scpiPath == "CONFIGURATION:SYST:NAMESESSION" ||
                        scpiPath == "CONFIGURATION:SYST:XSESSION";
        if (!skipHere && !scpiPath.isEmpty() && !type.isEmpty() && type.contains("Command")) {
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
            QTest::newRow(scpiPath.toLatin1()) << scpiPath << scpiParam;
        }
        iter.next();
    }
}

void test_scpi_all_responses::checkScpiCmdResponse()
{
    SCPIMODULE::cSCPIModule *scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());

    QFETCH(QString, scpiPath);
    QFETCH(QString, scpiParam);
    QString scpiCmd;
    if(!scpiParam.isEmpty())
        scpiCmd = scpiPath + " " + scpiParam + ";";
    else
        scpiCmd = scpiPath + ";";

    qInfo("%s", qPrintable(scpiCmd));
    QCOMPARE(client.getAtLeastOneResponse(), false);
    client.sendScpiCmds(scpiCmd);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(client.getAtLeastOneResponse(), true);
    QCOMPARE(client.getLastResponse().isNull(), true);
    QCOMPARE(client.getLastResponse().getStr(), "");
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
