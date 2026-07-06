#include "test_scpi_cmds_in_session.h"
#include "modulemanagerconfig.h"
#include "scpitestselectableclient.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <scpimodule.h>
#include <scpiserver.h>
#include <statusmodule.h>
#include <rangemodule.h>
#include <vf_entity_rpc_event_handler.h>
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_scpi_cmds_in_session::initTestCase_data()
{
    QTest::addColumn<ScpiTestSelectableClient::ClientType>("clientType");
    QTest::newRow("Test") << ScpiTestSelectableClient::TEST;
    QTest::newRow("Net") << ScpiTestSelectableClient::NET;
}

void test_scpi_cmds_in_session::initialSession()
{
    startModmanWithSession(":/session-scpi-only.json");
    VeinStorage::AbstractDatabase *veinStorageDb = m_testRunner->getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();
    QCOMPARE(entityList.count(), 2);

    QList<QString> componentList = veinStorageDb->getComponentList(9999);
    QCOMPARE(componentList.count(), 4); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile */
}

void test_scpi_cmds_in_session::initialTestClient()
{
    startModmanWithSession(":/session-scpi-only.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QString receive1 = client.sendReceive("*STB?");
    QString receive2 = client.sendReceive("*STB?");

    QCOMPARE(receive1, "+0");
    QCOMPARE(receive2, "+0");
}

void test_scpi_cmds_in_session::minScpiDevIface()
{
    ModulemanagerConfig::setDemoDevice("mt310s2");
    startModmanWithSession(":/session-scpi-only.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QString receive = client.sendReceive("dev:iface?", false);

    QFile ifaceBaseXmlFile("://dev-iface-basic.xml");
    QVERIFY(ifaceBaseXmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    XmlDocumentCompare compare;
    qInfo("%s", qPrintable(receive));
    QVERIFY(compare.compareXml(receive, ifaceBaseXmlFile.readAll(), true));
}

void test_scpi_cmds_in_session::initialScpiCommandsOnOtherModules()
{
    startModmanWithSession(":/session-two-modules.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QString receive1 = client.sendReceive("*STB?");
    QString receive2 = client.sendReceive("STATUS:DEV1:SERIAL?");
    QString receive3 = client.sendReceive("*STB?");
    QString receive4 = client.sendReceive("STATUS:DEV1:SERIAL?");
    QString receive5 = client.sendReceive("*STB?");

    QCOMPARE(receive1, "+0");
    QCOMPARE(receive2, "Unknown");
    QCOMPARE(receive3, "+0");
    QCOMPARE(receive4, "Unknown");
    QCOMPARE(receive5, "+0");
}

void test_scpi_cmds_in_session::multiReadDoubleDeleteCrasher()
{
    // * double delete fixed by 3766814ec0fae75ad7f18c7f71c34a767675e6e4.
    // * tested by reverting fix -> crashed
    startModmanWithSession(":/session-three-modules.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    // multi read to cause double delete crasher
    QString receive1 = client.sendReceive("CONFIGURATION:RNG1:RNGAUTO?");
    QString receive2 = client.sendReceive("CONFIGURATION:RNG1:GROUPING?");
    client.sendReceive("SENSE:RNG1:UL1:RANGE?");
    client.sendReceive("SENSE:RNG1:UL2:RANGE?");
    client.sendReceive("SENSE:RNG1:UL3:RANGE?");

    QCOMPARE(receive1, "0");
    QCOMPARE(receive2, "1");
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntities()
{
    startModmanWithSession(":/session-three-modules.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QString dumped = client.sendReceive("dev:iface?", false);
    QString expected = TestLogHelpers::loadFile(":/dev-iface-three.xml");

    XmlDocumentCompare compare;
    bool ok = compare.compareXml(expected, dumped);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiffFile(":/dev-iface-three.xml", dumped);
    QVERIFY(ok);
}

void test_scpi_cmds_in_session::closeSocketOnPendingWriteStbQueryNoCrasher()
{
    killModman(); // ensure fresh modman
    startModmanWithSession(":/range-min-session.json");
    ScpiModuleNetClientBlocked client; // This test is net client specific

    QString currRange = client.sendReceive("SENSE:RNG1:Il1:RANGE?");
    QCOMPARE(currRange, "10A");

    client.sendMulti(QByteArrayList() << "SENSE:RNG1:Il1:RANGE 25mA;" << "*STB?");
    client.closeSocket();
    TimeMachineObject::feedEventLoop();

    ScpiModuleNetClientBlocked clientCheck;
    currRange = clientCheck.sendReceive("SENSE:RNG1:Il1:RANGE?");
    QCOMPARE(currRange, "25mA");
}

void test_scpi_cmds_in_session::multilineCommandsLastOpc()
{
    startModmanWithSession(":/range-min-session.json");
    ScpiModuleNetClientBlocked client;
    QByteArrayList commands = QByteArrayList() << "SENSE:RNG1:UL1:RANGE 8V;|SENSE:RNG1:UL2:RANGE 8V;|SENSE:RNG1:UL3:RANGE 8V;|SENSE:RNG1:UAUX:RANGE 8V;"
                                               << "*OPC?";
    client.sendMulti(commands);
    TimeMachineObject::feedEventLoop();
    QByteArrayList responses = client.receiveMulti();
    QCOMPARE(responses.count(), 1);

    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE?"), "8V");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL2:RANGE?"), "8V");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL3:RANGE?"), "8V");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UAUX:RANGE?"), "8V");
}

void test_scpi_cmds_in_session::catalogFormat()
{
    startModmanWithSession(":/session-scpi-only.json");
    ScpiModuleNetClientBlocked client;
    QString sessionCatalog = client.sendReceive("CONFIGURATION:SYST:SESSION:CATALOG?");
    QCOMPARE(sessionCatalog, "Default;EMOB AC;EMOB DC;DC: 4*Voltage / 1*Current");
}

void test_scpi_cmds_in_session::executeRpcQueryWrongRpcName()
{
    startModmanWithSession(":/session-scpi-only.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("CALCULATE:EM01:0001:FOO?"), "");
}

void test_scpi_cmds_in_session::executeRpcReadLockStateQuery()
{
    startModmanWithSession(":/hotpluscontrols-min-session.json");
    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("EMOB:HOTP1:EMLOCKSTATE?"), "4");
}

void test_scpi_cmds_in_session::executeRpcQueryInvalidParams()
{
    startModmanWithSession("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    m_testRunner->getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    m_testRunner->start(":/session-scpi-only.json");
    TimeMachineObject::feedEventLoop();

    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("CALCULATE:RPC1? 7;"), "");
}

void test_scpi_cmds_in_session::executeRpcQueryOneParam()
{
    startModmanWithSession("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    m_testRunner->getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    m_testRunner->start(":/session-scpi-only.json");
    TimeMachineObject::feedEventLoop();

    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("CALCULATE:RPC1? true;"), "false");
}

void test_scpi_cmds_in_session::doNotExecuteRpcQueryMultipleParams()
{
    startModmanWithSession("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    m_testRunner->getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    m_testRunner->start(":/session-scpi-only.json");
    TimeMachineObject::feedEventLoop();

    QFETCH_GLOBAL(ScpiTestSelectableClient::ClientType, clientType);
    ScpiTestSelectableClient client(clientType, getScpiModule());

    QCOMPARE(client.sendReceive("CALCULATE:RPC2? foo;true;"), "");
}

void test_scpi_cmds_in_session::startModmanWithSession(const QString &sessionFile)
{
    if (m_testRunner == nullptr || sessionFile != m_testRunner->getSessionFileName()) {
        killModman();
        m_testRunner = std::make_unique<ModuleManagerTestRunner>(sessionFile);
    }
}

void test_scpi_cmds_in_session::killModman()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}

SCPIMODULE::cSCPIModule *test_scpi_cmds_in_session::getScpiModule()
{
    return static_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
}

