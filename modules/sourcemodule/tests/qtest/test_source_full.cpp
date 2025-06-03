#include "test_source_full.h"
#include <jsonparamapi.h>
#include <vs_dumpjson.h>
#include <vf_client_component_setter.h>
#include <mocktcpnetworkfactory.h>
#include <testloghelpers.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_source_full)

static int constexpr sourceEntityId = 1300;

void test_source_full::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_source_full::cleanup()
{
    if (m_testRunner) {
        m_testRunner->getModManFacade()->clearModuleSystems();
        m_testRunner->getModManFacade()->getEventHandler()->clearSubsystems();
    }
    if(m_netClient)
        m_netClient->getEventHandler()->clearSubsystems();
    TimeMachineObject::feedEventLoop();
    m_testRunner.reset();
    m_netClient.reset();
    m_entityItem.reset();
    m_serverTcpSystem.reset();
    m_serverNetSystem.reset();
}

void test_source_full::entity_avail_mt310s2()
{
    setupServerAndClient(":/sessions/minimal.json", "mt310s2");
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(sourceEntityId));
}

void test_source_full::entity_avail_mt581s2()
{
    setupServerAndClient(":/sessions/minimal.json", "mt581s2");
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(sourceEntityId));
}

void test_source_full::vein_dump_mt310s2()
{
    setupServerAndClient(":/sessions/minimal.json", "mt310s2");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitialMt310s2.json");
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();

    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << sourceEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_source_full::vein_dump_mt581s2()
{
    setupServerAndClient(":/sessions/minimal.json", "mt581s2");
    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitialMt581s2.json");
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();

    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << sourceEntityId);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_source_full::switch_on_mt310s2()
{
    QSKIP("TODO: Old RS232 implementation blocks system shutdown. ATTOW the task was to gather data. Mission accomplished.");
    setupServerAndClient(":/sessions/minimal.json", "mt310s2");
    setDemoSourceCount(1);
    QJsonObject jsonEventDump;
    setupServerAndClientSpies(jsonEventDump);

    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::StorageComponentPtr storedState = veinStorage->getDb()->findComponent(sourceEntityId, "PAR_SourceState0");
    JsonParamApi paramApi;
    QJsonObject oldValue = storedState->getValue().toJsonObject();
    paramApi.setParams(oldValue);
    paramApi.setOn(true);

    QEvent* event = VfClientComponentSetter::generateEvent(sourceEntityId, "PAR_SourceState0",
                                                           oldValue, paramApi.getParams());
    m_entityItem->sendEvent(event);
    TimeMachineForTest::getInstance()->processTimers(5000);

    QFile file(":/veinEventDumps/dumpSwitchOnMt310s2.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(jsonEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_source_full::switch_on_mt581s2()
{
    // What we need:
    // * Send load to server
    // * Demo server responds busy
    // * Demo Server rebonds

    setupServerAndClient(":/sessions/minimal.json", "mt581s2");
    QJsonObject jsonEventDump;
    setupServerAndClientSpies(jsonEventDump);

    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::StorageComponentPtr storedState = veinStorage->getDb()->findComponent(sourceEntityId, "PAR_SourceState0");
    JsonParamApi paramApi;
    QJsonObject oldValue = storedState->getValue().toJsonObject();
    paramApi.setParams(oldValue);
    paramApi.setOn(true);

    QEvent* event = VfClientComponentSetter::generateEvent(sourceEntityId, "PAR_SourceState0",
                                                           oldValue, paramApi.getParams());
    m_entityItem->sendEvent(event);
    TimeMachineForTest::getInstance()->processTimers(5000);

    QFile file(":/veinEventDumps/dumpSwitchOnMt581s2.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(jsonEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_source_full::setupServerAndClient(const QString &session, const QString &dut)
{
    constexpr int veinServerPort = 12000;
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(session, false, dut);
    m_serverNetSystem = std::make_unique<VeinNet::NetworkSystem>();
    m_serverNetSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);
    m_serverTcpSystem = std::make_unique<VeinNet::TcpSystem>(VeinTcp::MockTcpNetworkFactory::create());
    ModuleManagerSetupFacade *modmanFacade = m_testRunner->getModManFacade();
    modmanFacade->addSubsystem(m_serverNetSystem.get());
    modmanFacade->addSubsystem(m_serverTcpSystem.get());
    m_serverTcpSystem->startServer(veinServerPort);

    m_netClient = std::make_unique<VfCoreStackClient>(VeinTcp::MockTcpNetworkFactory::create());
    m_netClient->connectToServer("127.0.0.1", veinServerPort);
    TimeMachineObject::feedEventLoop();
    m_entityItem = VfEntityComponentEventItem::create(sourceEntityId);
    m_netClient->addItem(m_entityItem);
    m_netClient->subscribeEntity(sourceEntityId);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(m_serverNetSystem->getSubscriberCount(sourceEntityId), 1);
}

void test_source_full::setupServerAndClientSpies(QJsonObject &jsonEvents)
{
    ModuleManagerSetupFacade *modmanFacade = m_testRunner->getModManFacade();

    m_serverCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&jsonEvents, "server-enter");
    modmanFacade->prependModuleSystem(m_serverCmdEventSpyTop.get());
    /*m_serverCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&jsonEvents, "server-fallthrough");
    connect(modmanFacade->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_serverCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    modmanFacade->addSubsystem(m_serverCmdEventSpyBottom.get());*/

    m_clientCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&jsonEvents, "client-enter");
    m_netClient->prependEventSystem(m_clientCmdEventSpyTop.get());
    /*m_clientCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&jsonEvents, "client-fallthrough");
    connect(m_netClient->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_clientCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    m_netClient->appendEventSystem(m_clientCmdEventSpyBottom.get());*/
}

void test_source_full::setDemoSourceCount(int sourceCount)
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::StorageComponentPtr stored = veinStorage->getDb()->findComponent(sourceEntityId, "PAR_DemoSources");
    QVariant oldValue = stored->getValue();

    QEvent* event = VfClientComponentSetter::generateEvent(sourceEntityId, "PAR_DemoSources",
                                                           oldValue, sourceCount);
    m_entityItem->sendEvent(event);
    TimeMachineObject::feedEventLoop();
}
