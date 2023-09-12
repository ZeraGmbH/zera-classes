#include "test_scpi_cmds_in_session.h"
#include "modulemanagerfortest.h"
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

static int getEntityCount(ModuleManagerForTest *modman)
{
    VeinStorage::VeinHash* storageHash = modman->getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    return entityList.count();
}

void test_scpi_cmds_in_session::initialSession()
{   // First test on ModuleManagerForTest to know it works as expected
    ModuleManagerForTest modman;

    SCPIMODULE::cSCPIModule scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    QCOMPARE(getEntityCount(&modman), 1);
    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QList<QString> componentList = storageHash->getEntityComponents(entityList[0]);
    QCOMPARE(componentList.count(), 5); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile / ACT_DEV_IFACE
}

void test_scpi_cmds_in_session::initialTestClient()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 1);

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    client.sendScpiCmds("*STB?");
    client.sendScpiCmds("*STB?");

    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+0");
    QCOMPARE(responses[1], "+0");
}

void test_scpi_cmds_in_session::minScpiDevIface()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 1);

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    client.sendScpiCmds("dev:iface?");
    QCOMPARE(responses.count(), 1);
    QFile ifaceBaseXmlFile("://dev-iface-basic.xml");
    QVERIFY(ifaceBaseXmlFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly));
    XmlDocumentCompare compare;
    qInfo("%s", qPrintable(responses[0]));
    QVERIFY(compare.compareXml(responses[0], ifaceBaseXmlFile.readAll(), true));
}

void test_scpi_cmds_in_session::initialScpiCommandsOnOtherModules()
{
    ModuleManagerForTest modman;
    // !!! Pitfall: modules handled by scpimodule must be added first
    // Maybe a bit far fetched but: how about adding modulemanager in
    // zera-classes and use (demo) session files...
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem(), true);
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 2);

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    // UUhh more pitfall: SCPI base commands as "*STB?" are fired immediately.
    // Command on other modules but scpimodule - passing vein - require event loop
    // feeding.
    client.sendScpiCmds("*STB?");
    client.sendScpiCmds("STATUS:DEV1:SERIAL?");
    ModuleManagerForTest::feedEventLoop();
    client.sendScpiCmds("*STB?");
    client.sendScpiCmds("STATUS:DEV1:SERIAL?");
    ModuleManagerForTest::feedEventLoop();
    client.sendScpiCmds("*STB?");

    QCOMPARE(responses.count(), 5);
    QCOMPARE(responses[0], "+0");
    QCOMPARE(responses[1], "123456789");
    QCOMPARE(responses[2], "+0");
    QCOMPARE(responses[3], "123456789");
    QCOMPARE(responses[4], "+0");
}

void test_scpi_cmds_in_session::multiReadDoubleDeleteCrasher()
{
    // * double delete fixed by 3766814ec0fae75ad7f18c7f71c34a767675e6e4.
    // * tested by reverting fix -> crashed
    ModuleManagerForTest modman;
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem(), true);
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");
    RANGEMODULE::cRangeModule rangeModule(1, 1020, modman.getStorageSystem(), true);
    modman.addModule(&rangeModule, QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "mt310s2-rangemodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 3);

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    // multi read to cause double delete crasher
    client.sendScpiCmds("CONFIGURATION:RNG1:RNGAUTO?");
    client.sendScpiCmds("CONFIGURATION:RNG1:GROUPING?");
    client.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL2:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL3:RANGE?");
    ModuleManagerForTest::feedEventLoop();
    QCOMPARE(responses.count(), 5);
    QCOMPARE(responses[0], "0");
    QCOMPARE(responses[1], "1");
}

void test_scpi_cmds_in_session::devIfaceVeinComponent()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 1);

    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QList<QString> componentList = storageHash->getEntityComponents(entityList[0]);
    QVERIFY(componentList.contains("ACT_DEV_IFACE"));

    ModuleManagerForTest::feedEventLoop(); // for setup SCPI from vein

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    client.sendScpiCmds("dev:iface?");
    QString actDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE").toString();
    if(actDevIface.isEmpty()) // we have to make module resilient to this situation
        qFatal("ACT_DEV_IFACE empty - local modulemanager running???");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(actDevIface, responses[0], true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntities()
{
    ModuleManagerForTest modman;
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem(), true);
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");
    RANGEMODULE::cRangeModule rangeModule(1, 1020, modman.getStorageSystem(), true);
    modman.addModule(&rangeModule, QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "mt310s2-rangemodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 3);

    ModuleManagerForTest::feedEventLoop(); // for setup SCPI from vein

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    client.sendScpiCmds("dev:iface?");
    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QVariant xmlDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE");
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), responses[0], true));
}

void test_scpi_cmds_in_session::devIfaceVeinComponentMultipleEntitiesForLongXml()
{
    ModuleManagerForTest modman;
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem(), true);
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");

    std::vector<std::unique_ptr<RANGEMODULE::cRangeModule>> ptrList;
    for(int i=1; i<=9; i++) { // remember >=10 makes short names ambiguous
        ptrList.push_back(std::make_unique<RANGEMODULE::cRangeModule>(i, 1019+i, modman.getStorageSystem(), true));
        modman.addModule(ptrList.back().get(), QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "mt310s2-rangemodule.xml");
    }
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    ModuleManagerForTest::feedEventLoop(); // for setup SCPI from vein

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    client.sendScpiCmds("dev:iface?");
    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QVariant xmlDevIface = storageHash->getStoredValue(9999, "ACT_DEV_IFACE");
    // testing this on target / vf-debugger cutted string of len 67395 characters
    QVERIFY(xmlDevIface.toString().size() >= 67395);
    qInfo("%i", xmlDevIface.toString().size());
    XmlDocumentCompare compare;
    QVERIFY(compare.compareXml(xmlDevIface.toString(), responses[0], true));
}

/*void test_scpi_cmds_in_session::workWithRangemodule()
{
    ModuleManagerForTest modman;
    RANGEMODULE::cRangeModule rangeModule(1, 1020, modman.getStorageSystem());
    modman.addModule(&rangeModule, QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "mt310s2-rangemodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");
    QCOMPARE(getEntityCount(&modman), 2);

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    // check initial state
    client.sendScpiCmds("CONFIGURATION:RNG1:RNGAUTO?");
    client.sendScpiCmds("CONFIGURATION:RNG1:GROUPING?");
    client.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL2:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL3:RANGE?");
    ModuleManagerForTest::feedEventLoop();
    QCOMPARE(responses.count(), 5);
    QCOMPARE(responses[0], "0"); // rngauto
    QCOMPARE(responses[1], "1"); // grouping
    QCOMPARE(responses[2], "480V");
    QCOMPARE(responses[3], "480V");
    QCOMPARE(responses[4], "480V");

    // change some
    client.sendScpiCmds("CONFIGURATION:RNG1:GROUPING 0;");
    client.sendScpiCmds("SENSE:RNG1:UL1:RANGE 240V;");
    client.sendScpiCmds("SENSE:RNG1:UL3:RANGE 60V;");
    client.sendScpiCmds("SENSE:RNG1:UL2:RANGE 120V;");
    ModuleManagerForTest::feedEventLoop();

    // check changes
    responses.clear();
    client.sendScpiCmds("CONFIGURATION:RNG1:RNGAUTO?");
    client.sendScpiCmds("CONFIGURATION:RNG1:GROUPING?");
    client.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL2:RANGE?");
    client.sendScpiCmds("SENSE:RNG1:UL3:RANGE?");
    ModuleManagerForTest::feedEventLoop(); // pitfall see initialScpiCommandsOnOtherModules on details
    client.sendScpiCmds("*STB?");
    QCOMPARE(responses.count(), 6);
    QCOMPARE(responses[5], "+0");
    QCOMPARE(responses[0], "0"); // rngauto
    QCOMPARE(responses[1], "0"); // grouping
    QCOMPARE(responses[2], "240V");
    QCOMPARE(responses[3], "120V");
    QCOMPARE(responses[4], "60V");
}*/
