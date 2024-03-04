#include "test_scpi_queue.h"
#include <rangemodule.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <scpimodulefortest.h>
#include <QTest>

QTEST_MAIN(test_scpi_queue)

void enableQueuing(SCPIMODULE::cSCPIInterface* interface)
{
    interface->setEnableQueue(true);
}

void disableQueuing(SCPIMODULE::cSCPIInterface* interface)
{
    interface->setEnableQueue(false);
}

void test_scpi_queue::initTestCase()
{
    m_serviceInterfaceFactory = std::make_shared<DemoFactoryServiceInterfaces>();
    TimerFactoryQtForTest::enableTest();
}

void test_scpi_queue::cleanup()
{
    if(m_modMan)
        m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    TimeMachineObject::feedEventLoop();
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
}


void test_scpi_queue::sendStandardCmdsQueueDisabledAndEnabled()
{
    setupServices(":/session-scpi-only.json");

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);

    SCPIMODULE::ScpiTestClient *client = new SCPIMODULE::ScpiTestClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiModule->getSCPIServer()->appendClient(client);

    QStringList responses;
    connect(client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, client, [&responses] (QString response) {
        responses.append(response);
    });

    disableQueuing(client->getScpiInterface());
    client->sendScpiCmds("*SRE 2;|*SRE?|*IDN?");
    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+2");
    QVERIFY(responses[1].contains("DEMO"));

    responses.clear();
    enableQueuing(client->getScpiInterface());
    client->sendScpiCmds("*ESE?|*RST|*ESR?");
    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+0");
    QCOMPARE(responses[1], "+0");
}

void test_scpi_queue::sendErroneousAndCorrectStandardCmds()
{
    setupServices(":/session-scpi-only.json");

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);

    SCPIMODULE::ScpiTestClient *client = new SCPIMODULE::ScpiTestClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiModule->getSCPIServer()->appendClient(client);

    QStringList responses;
    connect(client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, client, [&responses] (QString response) {
        responses.append(response);
    });

    enableQueuing(client->getScpiInterface());
    client->sendScpiCmds("*CLS?|*OPC?|*IDN|*TST|*stb?");

    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+1");
    QCOMPARE(responses[1], "+4");
}

void test_scpi_queue::sendSubSystemAndStandardCommands()
{
    setupServices(":/session-scpi-and-range.json");

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);

    SCPIMODULE::ScpiTestClient *client = new SCPIMODULE::ScpiTestClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiModule->getSCPIServer()->appendClient(client);

    QStringList responses;
    connect(client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, client, [&responses] (QString response) {
        responses.append(response);
    });

    disableQueuing(client->getScpiInterface());
    client->sendScpiCmds("MEASURE:RNG1:F?|MEASURE:RNG1:UL1?|MEASURE:RNG1:UL2?|*OPC?");
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(2000);

    QCOMPARE(responses.count(), 4);
    QCOMPARE(responses[0], "+1");
    QVERIFY(responses[1].startsWith("RNG1:")); //"RNG1:F:[Hz]:14.016426049618836;"

    responses.clear();
    enableQueuing(client->getScpiInterface());
    client->sendScpiCmds("MEASURE:RNG1:F?|MEASURE:RNG1:UL1?|MEASURE:RNG1:UL2?|*OPC?");
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(2000);

    QCOMPARE(responses.count(), 4);
    QVERIFY(responses[0].startsWith("RNG1"));
    QCOMPARE(responses[3], "+1");
}

void test_scpi_queue::enableAndDisableQueueWhileExecutingCmds()
{
    setupServices(":/session-scpi-and-range.json");

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);

    SCPIMODULE::ScpiTestClient *client = new SCPIMODULE::ScpiTestClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiModule->getSCPIServer()->appendClient(client);

    QStringList responses;
    connect(client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, client, [&responses] (QString response) {
        responses.append(response);
    });

    enableQueuing(client->getScpiInterface());
    client->sendScpiCmds("MEASURE:RNG1:UL1?|MEASURE:RNG1:UL2?|MEASURE:RNG1:UL3?|MEASURE:RNG1:IL1?|MEASURE:RNG1:IL2?|MEASURE:RNG1:IL3?|*OPC?|*IDN?");
    TimeMachineObject::feedEventLoop();
    TimeMachineForTest::getInstance()->processTimers(1500);
    QCOMPARE(responses.count(), 3);

    disableQueuing(client->getScpiInterface());
    TimeMachineForTest::getInstance()->processTimers(2000);

    QCOMPARE(responses.count(), 8);
    QCOMPARE(responses[4], "+1");  //Since the standard cmds are quick, once we disable the queue they respond first
    QVERIFY(responses[5].contains("DEMO"));
}

void test_scpi_queue::disableAndEnableQueueWhileExecutingCmds()
{
    setupServices(":/session-scpi-and-range.json");

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(m_modMan->getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);

    SCPIMODULE::ScpiTestClient *client = new SCPIMODULE::ScpiTestClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiModule->getSCPIServer()->appendClient(client);

    QStringList responses;
    connect(client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, client, [&responses] (QString response) {
        responses.append(response);
    });

    disableQueuing(client->getScpiInterface());
    client->sendScpiCmds("MEASURE:RNG1:UL1?|MEASURE:RNG1:UL2?|MEASURE:RNG1:UL3?|MEASURE:RNG1:IL1?|MEASURE:RNG1:IL2?|MEASURE:RNG1:IL3?|*OPC?|*IDN?");
    TimeMachineForTest::getInstance()->processTimers(100);
    QCOMPARE(responses.count(), 2); //the 2 last cmds are quick

    enableQueuing(client->getScpiInterface());
    TimeMachineForTest::getInstance()->processTimers(399);
    QCOMPARE(responses.count(), 2);
    TimeMachineForTest::getInstance()->processTimers(1);
    QCOMPARE(responses.count(), 8);
}

QByteArray test_scpi_queue::loadConfig(QString configFileNameFull)
{
    QFile file(configFileNameFull);
    if(file.open(QIODevice::ReadOnly))
        return file.readAll();
    return QByteArray();
}

void test_scpi_queue::setupServices(QString sessionFileName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), m_serviceInterfaceFactory);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllTestServices("mt310s2");
    m_modMan->loadSession(sessionFileName);
    m_modMan->waitUntilModulesAreReady();
}
