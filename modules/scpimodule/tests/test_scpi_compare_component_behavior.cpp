#include "test_scpi_compare_component_behavior.h"
#include "scpimodulenetclientblocked.h"
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_scpi_compare_component_behavior)

static int constexpr serverPort = 4712;
static constexpr int systemEntityId = 0;

void test_scpi_compare_component_behavior::initTestCase()
{
    TimerFactoryQtForTest::enableTest();

    m_testRunner = std::make_unique<ModuleManagerTestRunner>("mt310s2-meas-session.json");
    setupSpy();
}

void test_scpi_compare_component_behavior::cleanupTestCase()
{
    m_testRunner.reset(); // before extra event systems
}

void test_scpi_compare_component_behavior::cleanup()
{
    m_veinEventDump = QJsonObject();
}

void test_scpi_compare_component_behavior::setupSpy()
{
    ModuleManagerSetupFacade* modManFacade = m_testRunner->getModManFacade();
    m_serverCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-enter");
    modManFacade->prependModuleSystem(m_serverCmdEventSpyTop.get());

    m_serverCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-fallthrough");
    connect(modManFacade->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_serverCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    modManFacade->addSubsystem(m_serverCmdEventSpyBottom.get());
}

void test_scpi_compare_component_behavior::queryOtherEntity()
{
    ScpiModuleNetClientBlocked client;

    QByteArray response = client.sendReceive("CONFIGURATION:SAM1:PLLAUTO?");
    QCOMPARE(response, "1");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpqueryOtherEntity.json", jsonDumped));
}

void test_scpi_compare_component_behavior::querySystemEntity()
{
    ScpiModuleNetClientBlocked client;

    QByteArray response = client.sendReceive("CONFIGURATION:SYST:LOGSCPIFULL?");
    QCOMPARE(response, "1");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpquerySystemEntity.json", jsonDumped));
}

void test_scpi_compare_component_behavior::cmdOtherEntity()
{
    ScpiModuleNetClientBlocked client;

    client.sendReceive("CONFIGURATION:SAM1:PLLAUTO 1;");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpcmdOtherEntity.json", jsonDumped));
}

void test_scpi_compare_component_behavior::cmdSystemEntity()
{
    ScpiModuleNetClientBlocked client;

    client.sendReceive("CONFIGURATION:SYST:LOGSCPIFULL 1;");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpcmdSystemEntity.json", jsonDumped));
}

void test_scpi_compare_component_behavior::cmdInvalidOtherEntity()
{
    ScpiModuleNetClientBlocked client;

    client.sendReceive("CONFIGURATION:SAM1:PLLAUTO FOO;");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpcmdInvalidOtherEntity.json", jsonDumped));
}

void test_scpi_compare_component_behavior::cmdInvalidSystemEntity()
{
    ScpiModuleNetClientBlocked client;

    client.sendReceive("CONFIGURATION:SYST:LOGSCPIFULL FOO;");

    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/vein-event-dumps/dumpcmdInvalidSystemEntity.json", jsonDumped));
}
