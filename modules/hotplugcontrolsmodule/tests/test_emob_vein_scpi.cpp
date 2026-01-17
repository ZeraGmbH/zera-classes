#include "test_emob_vein_scpi.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <vf_client_rpc_invoker.h>
#include <xmldocumentcompare.h>
#include <vs_dumpjson.h>
#include "controllerpersitentdata.h"
#include <emobdefinitions.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_emob_vein_scpi)

static int constexpr hotplugControlsEntityId = 1700;
static int constexpr serverPort = 4711;
static constexpr int systemEntityId = 0;

void test_emob_vein_scpi::invokeInvalidRpcNameScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:FOO?");
    QCOMPARE(status, "");
    status = m_scpiClient->sendReceive("*stb?");
    QCOMPARE(status, "+4");

    QFile file(":/vein-event-dumps/dumpinvokeInvalidRpcNameScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::activateEmobPushButtonNoEmobNoParamScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS;|*stb?");
    QCOMPARE(status, "+4"); // no emob

    QFile file(":/vein-event-dumps/dumpActivateEmobPushButtonNoParamScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::activateEmobPushNoParamOneEmob()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS;|*stb?");
    QCOMPARE(status, "+0");
}

void test_emob_vein_scpi::activateEmobPushButtonWithParamScpi()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IAUX");

    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS IAUX;|*stb?");
    QCOMPARE(status, "+0");

    QFile file(":/vein-event-dumps/dumpActivateEmobPushButtonWithParamScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::activateEmobPushNoParamMutipleHotplug()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS;|*stb?");
    QCOMPARE(status, "+4");
}

void test_emob_vein_scpi::activateEmobPushInvalidParam()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS FOO;|*stb?");
    QCOMPARE(status, "+4");
}

void test_emob_vein_scpi::activateEmobPushIncorrectParam()
{
    //valid but incorrect param
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS IL1;|*stb?");
    QCOMPARE(status, "+4");
}

void test_emob_vein_scpi::activateEmobPushValidParamMutipleHotplug()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:PBPRESS IL3;|*stb?");
    QCOMPARE(status, "+0");
}

void test_emob_vein_scpi::activateEmobPushButtonVein()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IAUX");

    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    invokeRpc("RPC_activatePushButton", "p_channelName", "IAUX");
    TimeMachineObject::feedEventLoop();

    QFile file(":/vein-event-dumps/dumpActivateEmobPushButtonVein.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
    QCOMPARE(spyRpcFinish.count(), 1);
}

void test_emob_vein_scpi::readLockStateCorrectRpcNameScpi()
{
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE?");
    QCOMPARE(status, QString::number(reademoblockstate::emobstate_error));

    QFile file(":/vein-event-dumps/dumpReadLockStateCorrectRpcNameScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateScpiNoParamOneEmob()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE?");
    QCOMPARE(status, QString::number(reademoblockstate::emobstate_open));

    QFile file(":/vein-event-dumps/dumpReadLockStateNoParamOneEmobScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateScpiNoParamMutipleHotplug()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status1 = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE?");
    QCOMPARE(status1, QString::number(reademoblockstate::emobstate_error));

    QFile file(":/vein-event-dumps/dumpReadLockStateNoParamMutipleHotplugScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateScpiInvalidParam()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE? FOO;");
    QCOMPARE(status, "");
    status = m_scpiClient->sendReceive("*stb?");
    QCOMPARE(status, "+4");

    QFile file(":/vein-event-dumps/dumpReadLockStateInvalidParamScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateScpiIncorrectParam()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE? IL1;");
    QCOMPARE(status, QString::number(reademoblockstate::emobstate_error));
}

void test_emob_vein_scpi::readLockStateScpiValidParamMutipleHotplug()
{
    m_testRunner->addStandardEmobControllers(QStringList()  << "IL3" << "IAUX");
    QString status = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE? IAUX;");
    QCOMPARE(status, QString::number(reademoblockstate::emobstate_open));

    QFile file(":/vein-event-dumps/dumpReadLockStateValidParamMutipleHotplugScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateTwiceScpi()
{
    m_testRunner->addStandardEmobControllers(QStringList() << "IAUX");
    QString status1 = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE?");
    QString status2 = m_scpiClient->sendReceive("EMOB:HOTP1:EMLOCKSTATE?");
    QCOMPARE(status1, QString::number(reademoblockstate::emobstate_open));
    QCOMPARE(status1, status2);

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceScpi.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::readLockStateTwiceVein()
{
    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    invokeRpc("RPC_readLockState", "p_channelName", "");
    TimeMachineObject::feedEventLoop();
    invokeRpc("RPC_readLockState", "p_channelName", "");
    TimeMachineObject::feedEventLoop();

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceVein.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
    QCOMPARE(spyRpcFinish.count(), 2);
}

void test_emob_vein_scpi::readLockStateTwiceVeinFullQueue()
{
    QSignalSpy spyRpcFinish(m_rpcInvoker.get(), &VfRPCInvoker::sigRPCFinished);
    invokeRpc("RPC_readLockState", "p_channelName", "");
    invokeRpc("RPC_readLockState", "p_channelName", "");
    TimeMachineObject::feedEventLoop();

    QFile file(":/vein-event-dumps/dumpReadLockStateTwiceVeinFullQueue.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray jsonExpected = file.readAll();
    QByteArray jsonDumped = TestLogHelpers::dump(m_veinEventDump);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
    QCOMPARE(spyRpcFinish.count(), 2);
}

void test_emob_vein_scpi::dumpDevIface()
{
    QString dumped = m_scpiClient->sendReceive("dev:iface?", false);
    QString expected = TestLogHelpers::loadFile("://scpi-dump.xml");
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_emob_vein_scpi::dumpVeinInfModuleInterface()
{
    VeinStorage::AbstractDatabase *veinStorageDb = m_testRunner->getVeinStorageDb();
    VeinStorage::AbstractDatabase* storageDb = veinStorageDb;
    QByteArray jsonDumped = storageDb->getStoredValue(hotplugControlsEntityId, "INF_ModuleInterface").toByteArray();
    QString jsonExpected = TestLogHelpers::loadFile("://vein-inf-moduleinterface-dump.json");

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_emob_vein_scpi::init()
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

void test_emob_vein_scpi::cleanup()
{
    m_testRunner.reset();
    m_scpiClient.reset();
    m_veinClientStack.reset();
    m_netSystem.reset();
    m_tcpSystem.reset();

    m_veinEventDump = QJsonObject();
    ControllerPersitentData::cleanupPersitentData();
}

void test_emob_vein_scpi::invokeRpc(QString rpcName, QString paramName, QVariant paramValue)
{
    QVariantMap rpcParams;
    if(!paramName.isEmpty())
        rpcParams.insert(paramName, paramValue);
    m_rpcInvoker->invokeRPC(rpcName, rpcParams);
}

void test_emob_vein_scpi::setupSpy()
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
