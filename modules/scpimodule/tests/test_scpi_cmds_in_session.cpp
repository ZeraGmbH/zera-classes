#include "test_scpi_cmds_in_session.h"
#include "modulemanagertestrunner.h"
#include "modulemanagerconfig.h"
#include "scpimoduleclientblocked.h"
#include <timemachineobject.h>
#include <scpimodule.h>
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <statusmodule.h>
#include <rangemodule.h>
#include <vf_entity_rpc_event_handler.h>
#include <xmldocumentcompare.h>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::initialSession()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);

    QList<QString> componentList = veinStorage->getDb()->getComponentList(9999);
    QCOMPARE(componentList.count(), 6); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile / ACT_DEV_IFACE / PAR_OptionalScpiQueue*/
}

void test_scpi_cmds_in_session::initialTestClient()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");

    ScpiModuleClientBlocked client;
    QString receive1 = client.sendReceive("*STB?");
    QString receive2 = client.sendReceive("*STB?");

    QCOMPARE(receive1, "+0");
    QCOMPARE(receive2, "+0");
}

void test_scpi_cmds_in_session::minScpiDevIface()
{
    ModulemanagerConfig::setDemoDevice("mt310s2");
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?", false);

    QFile ifaceBaseXmlFile("://dev-iface-basic.xml");
    QVERIFY(ifaceBaseXmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    XmlDocumentCompare compare;
    qInfo("%s", qPrintable(receive));
    QVERIFY(compare.compareXml(receive, ifaceBaseXmlFile.readAll(), true));
}

void test_scpi_cmds_in_session::initialScpiCommandsOnOtherModules()
{
    ModuleManagerTestRunner testRunner(":/session-two-modules.json");

    ScpiModuleClientBlocked client;
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
    ModuleManagerTestRunner testRunner(":/session-three-modules.json");

    // multi read to cause double delete crasher
    ScpiModuleClientBlocked client;
    QString receive1 = client.sendReceive("CONFIGURATION:RNG1:RNGAUTO?");
    QString receive2 = client.sendReceive("CONFIGURATION:RNG1:GROUPING?");
    client.sendReceive("SENSE:RNG1:UL1:RANGE?");
    client.sendReceive("SENSE:RNG1:UL2:RANGE?");
    client.sendReceive("SENSE:RNG1:UL3:RANGE?");

    QCOMPARE(receive1, "0");
    QCOMPARE(receive2, "1");
}

void test_scpi_cmds_in_session::devIfaceVeinComponent()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");

    VeinStorage::AbstractDatabase* storageDb = testRunner.getVeinStorageSystem()->getDb();
    QList<QString> componentList = storageDb->getComponentList(9999);
    QVERIFY(componentList.contains("ACT_DEV_IFACE"));

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");
    QString actDevIface = storageDb->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(actDevIface, receive, true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntities()
{
    ModuleManagerTestRunner testRunner(":/session-three-modules.json");

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");

    VeinStorage::AbstractDatabase* storageDb = testRunner.getVeinStorageSystem()->getDb();
    QVariant xmlDevIface = storageDb->getStoredValue(9999, "ACT_DEV_IFACE");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), receive, true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntitiesForLongXml()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");

    VeinStorage::AbstractDatabase* storageDb = testRunner.getVeinStorageSystem()->getDb();
    QVariant xmlDevIface = storageDb->getStoredValue(9999, "ACT_DEV_IFACE");
    // testing this on target / vf-debugger cutted string of len 67395 characters
    QVERIFY(xmlDevIface.toString().size() >= 67395);
    qInfo("%i", xmlDevIface.toString().size());
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), receive, true));
}

void test_scpi_cmds_in_session::closeSocketOnPendingWriteStbQueryNoCrasher()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ScpiModuleClientBlocked client;
    QString currRange = client.sendReceive("SENSE:RNG1:Il1:RANGE?");
    QCOMPARE(currRange, "10A");

    client.sendMulti(QByteArrayList() << "SENSE:RNG1:Il1:RANGE 25mA;" << "*STB?");
    client.closeSocket();
    TimeMachineObject::feedEventLoop();

    ScpiModuleClientBlocked clientCheck;
    currRange = clientCheck.sendReceive("SENSE:RNG1:Il1:RANGE?");
    QCOMPARE(currRange, "25mA");
}

void test_scpi_cmds_in_session::multilineCommandsLastOpc()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ScpiModuleClientBlocked client;
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
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");
    ScpiModuleClientBlocked client;
    QString sessionCatalog = client.sendReceive("CONFIGURATION:SYST:SESSION:CATALOG?");
    QCOMPARE(sessionCatalog, "Default;EMOB AC;EMOB DC;DC: 4*Voltage / 1*Current");
}

void test_scpi_cmds_in_session::executeRpcQueryWrongRpcName()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:RPC_FOO()?");
    QCOMPARE(status, "");
}

void test_scpi_cmds_in_session::executeRpcReadLockStateQuery()
{
    ModuleManagerTestRunner testRunner(":/mt310s2-meas-session.json");
    ScpiModuleClientBlocked client;
    QString status = client.sendReceive("CALCULATE:EM01:0001:EMLOCKSTATE?");
    QCOMPARE(status, "4");
}

void test_scpi_cmds_in_session::executeRpcQueryInvalidParams()
{
    ModuleManagerTestRunner testRunner("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    testRunner.getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    testRunner.start(":/mt310s2-meas-session.json");
    TimeMachineObject::feedEventLoop();

    ScpiModuleClientBlocked client;
    QString answer = client.sendReceive("CALCULATE:RPC_FORTEST(p_param)? 7;");
    QCOMPARE(answer, "");
}

void test_scpi_cmds_in_session::executeRpcQueryOneParam()
{
    ModuleManagerTestRunner testRunner("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    testRunner.getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    testRunner.start(":/mt310s2-meas-session.json");
    TimeMachineObject::feedEventLoop();

    ScpiModuleClientBlocked client;
    QString answer = client.sendReceive("CALCULATE:RPC1? true;");
    QCOMPARE(answer, "false");
}

void test_scpi_cmds_in_session::executeRpcQueryParams()
{
    ModuleManagerTestRunner testRunner("");
    vfEntityRpcEventHandler *rpcEventHandler = new vfEntityRpcEventHandler();
    testRunner.getModManFacade()->addSubsystem(rpcEventHandler->getVeinEntity());
    rpcEventHandler->initOnce();
    TimeMachineObject::feedEventLoop();
    testRunner.start(":/mt310s2-meas-session.json");
    TimeMachineObject::feedEventLoop();

    ScpiModuleClientBlocked client;
    QString answer = client.sendReceive("CALCULATE:RPC2? foo;true;");
    QCOMPARE(answer, "foo");
}

