#include "test_scpi_cmds_in_session.h"
#include "licensesystemmock.h"
#include "testfactoryactvalmaninthemiddle.h"
#include "testmodulemanager.h"
#include "scpimoduleclientblocked.h"
#include <timemachineobject.h>
#include <scpimodule.h>
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <statusmodule.h>
#include <rangemodule.h>
#include <xmldocumentcompare.h>
#include <vector>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::cleanup()
{
    m_modMan->destroyModulesAndWaitUntilAllShutdown();
    m_modMan = nullptr;
    m_modmanFacade = nullptr;
    m_licenseSystem = nullptr;
}

void test_scpi_cmds_in_session::initialSession()
{
    setupServices(":/session-scpi-only.json");

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);

    QList<QString> componentList = storageHash->getEntityComponents(9999);
    QCOMPARE(componentList.count(), 6); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile / ACT_DEV_IFACE / PAR_OptionalScpiQueue*/
}

void test_scpi_cmds_in_session::initialTestClient()
{
    setupServices(":/session-scpi-only.json");

    ScpiModuleClientBlocked client;
    QString receive1 = client.sendReceive("*STB?");
    QString receive2 = client.sendReceive("*STB?");

    QCOMPARE(receive1, "+0");
    QCOMPARE(receive2, "+0");
}

void test_scpi_cmds_in_session::minScpiDevIface()
{
    setupServices(":/session-scpi-only.json");

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
    setupServices(":/session-two-modules.json");

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
    setupServices(":/session-three-modules.json");

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
    setupServices(":/session-scpi-only.json");

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QList<QString> componentList = storageHash->getEntityComponents(9999);
    QVERIFY(componentList.contains("ACT_DEV_IFACE"));

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");
    QString actDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(actDevIface, receive, true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntities()
{
    setupServices(":/session-three-modules.json");

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QVariant xmlDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), receive, true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntitiesForLongXml()
{
    setupServices(":/session-many-modules.json");

    ScpiModuleClientBlocked client;
    QString receive = client.sendReceive("dev:iface?");

    VeinStorage::VeinHash* storageHash = m_modmanFacade->getStorageSystem();
    QVariant xmlDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE");
    // testing this on target / vf-debugger cutted string of len 67395 characters
    QVERIFY(xmlDevIface.toString().size() >= 67395);
    qInfo("%i", xmlDevIface.toString().size());
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), receive, true));
}

void test_scpi_cmds_in_session::setupServices(QString sessionFileName)
{
    m_licenseSystem = std::make_unique<LicenseSystemMock>();
    m_modmanFacade = std::make_unique<ModuleManagerSetupFacade>(m_licenseSystem.get());
    m_modMan = std::make_unique<TestModuleManager>(m_modmanFacade.get(), std::make_shared<TestFactoryActValManInTheMiddle>(), true);
    m_modMan->loadAllAvailableModulePlugins();
    m_modMan->setupConnections();
    m_modMan->startAllServiceMocks("mt310s2");
    m_modMan->loadSession(sessionFileName);
    TimeMachineObject::feedEventLoop();
}
