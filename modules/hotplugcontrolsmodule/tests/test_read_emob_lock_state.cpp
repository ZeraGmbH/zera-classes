#include "test_read_emob_lock_state.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <vf_client_rpc_invoker.h>
#include <xmldocumentcompare.h>
#include <vs_dumpjson.h>
#include "controllerpersitentdata.h"
#include <emobdefinitions.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_read_emob_lock_state)

static int constexpr hotplugControlsEntityId = 1700;
static int constexpr serverPort = 4711;
static constexpr int systemEntityId = 0;

void test_read_emob_lock_state::readEmobPushButtonValue()
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::AbstractDatabase *dataBase = veinStorage->getDb();
    QList<int> entityList = dataBase->getEntityList();
    QVERIFY(entityList.contains(hotplugControlsEntityId));

    QVERIFY(dataBase->hasStoredValue(hotplugControlsEntityId, "PAR_EmobPushButton"));
    QVariant pressButtonValue = dataBase->getStoredValue(hotplugControlsEntityId, "PAR_EmobPushButton");
    QCOMPARE(pressButtonValue.toString(), "0");
}

void test_read_emob_lock_state::pressAndReadEmobPushButtonValue()
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::AbstractDatabase *dataBase = veinStorage->getDb();

    QVariant oldValue = dataBase->getStoredValue(hotplugControlsEntityId, "PAR_EmobPushButton");
    QCOMPARE(oldValue, 0);

    m_testRunner->setVfComponent(hotplugControlsEntityId, "PAR_EmobPushButton", 1);
    QCOMPARE(dataBase->getStoredValue(hotplugControlsEntityId, "PAR_EmobPushButton"), 0);
}

void test_read_emob_lock_state::readLockStateWrongRpcNameScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOT1:FOO?");
    QCOMPARE(status, "");

    QFile file(":/vein-event-dumps/dumpReadLockStateWrongRpcNameScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_emob_lock_state::readLockStateCorrectRpcNameScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOT1:EMLOCKSTATE?");
    QCOMPARE(status, "4");

    QFile file(":/vein-event-dumps/dumpReadLockStateCorrectRpcNameScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_emob_lock_state::readLockStateTwiceScpi()
{
    m_testRunner->fireHotplugInterrupt(QStringList()  << "IAUX");
    QString status1 = m_scpiClient->sendReceive("EMOB:HOT1:EMLOCKSTATE?");
    QString status2 = m_scpiClient->sendReceive("EMOB:HOT1:EMLOCKSTATE?");
    QCOMPARE(status1, QString::number(reademoblockstate::emobstate_open));
    QCOMPARE(status1, status2);

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_emob_lock_state::readLockStateTwiceVein()
{
    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    invokeRpc("RPC_readLockState", "", 0);
    TimeMachineObject::feedEventLoop();
    invokeRpc("RPC_readLockState", "", 0);
    TimeMachineObject::feedEventLoop();

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceVein.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
    QCOMPARE(spyRpcFinish.count(), 2);
}

void test_read_emob_lock_state::readLockStateTwiceVeinFullQueue()
{
    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    invokeRpc("RPC_readLockState", "", 0);
    invokeRpc("RPC_readLockState", "", 0);
    TimeMachineObject::feedEventLoop();

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceVeinFullQueue.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
    QCOMPARE(spyRpcFinish.count(), 2);
}

void test_read_emob_lock_state::dumpDevIface()
{
    QString dumped = m_scpiClient->sendReceive("dev:iface?", false);
    QString expected = TestLogHelpers::loadFile("://scpi-dump.xml");
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_read_emob_lock_state::dumpVeinInfModuleInterface()
{
    VeinStorage::AbstractEventSystem* veinStorage = m_testRunner->getVeinStorageSystem();
    VeinStorage::AbstractDatabase* storageDb = veinStorage->getDb();
    QByteArray jsonDumped = storageDb->getStoredValue(hotplugControlsEntityId, "INF_ModuleInterface").toByteArray();
    QString jsonExpected = TestLogHelpers::loadFile("://vein-inf-moduleinterface-dump.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_read_emob_lock_state::init()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/hotpluscontrols-min-session.json");
    m_netSystem = std::make_unique<VeinNet::NetworkSystem>();
    m_netSystem->setOperationMode(VeinNet::NetworkSystem::VNOM_SUBSCRIPTION);
    m_tcpSystem = std::make_unique<VeinNet::TcpSystem>(VeinTcp::MockTcpNetworkFactory::create());
    ModuleManagerSetupFacade* modManFacade = m_testRunner->getModManFacade();
    modManFacade->addSubsystem(m_netSystem.get());
    modManFacade->addSubsystem(m_tcpSystem.get());
    m_tcpSystem->startServer(serverPort);

    m_veinClientStack = std::make_unique<VfCoreStackClient>(VeinTcp::MockTcpNetworkFactory::create());
    m_veinClientStack->connectToServer("127.0.0.1", serverPort);
    m_veinClientStack->subscribeEntity(systemEntityId);
    m_veinClientStack->subscribeEntity(hotplugControlsEntityId);
    m_rpcInvoker = std::make_shared<VfRPCInvoker>(hotplugControlsEntityId, std::make_unique<VfClientRPCInvoker>());
    m_veinClientStack->addItem(m_rpcInvoker);
    TimeMachineObject::feedEventLoop();

    setupSpy();

    m_scpiClient = std::make_unique<ScpiModuleClientBlocked>();
}

void test_read_emob_lock_state::cleanup()
{
    m_testRunner.reset();
    m_scpiClient.reset();
    m_veinClientStack.reset();
    m_netSystem.reset();
    m_tcpSystem.reset();

    m_veinEventDump = QJsonObject();
    ControllerPersitentData::cleanupPersitentData();
}

QUuid test_read_emob_lock_state::invokeRpc(QString rpcName, QString paramName, QVariant paramValue)
{
    QVariantMap rpcParams;
    if(!paramName.isEmpty())
        rpcParams.insert(paramName, paramValue);
    QUuid id = m_rpcInvoker->invokeRPC(rpcName, rpcParams);
    return id;
}

void test_read_emob_lock_state::setupSpy()
{
    ModuleManagerSetupFacade* modManFacade = m_testRunner->getModManFacade();
    m_serverCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-enter");
    modManFacade->prependModuleSystem(m_serverCmdEventSpyTop.get());

    m_serverCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "server-fallthrough");
    connect(modManFacade->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_serverCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    modManFacade->addSubsystem(m_serverCmdEventSpyBottom.get());

    m_clientCmdEventSpyTop = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "client-enter");
    m_veinClientStack->prependEventSystem(m_clientCmdEventSpyTop.get());

    m_clientCmdEventSpyBottom = std::make_unique<TestJsonSpyEventSystem>(&m_veinEventDump, "client-fallthrough");
    connect(m_veinClientStack->getEventHandler(), &VeinEvent::EventHandler::sigEventAccepted,
            m_clientCmdEventSpyBottom.get(), &TestJsonSpyEventSystem::onEventAccepted);
    m_veinClientStack->appendEventSystem(m_clientCmdEventSpyBottom.get());
}
