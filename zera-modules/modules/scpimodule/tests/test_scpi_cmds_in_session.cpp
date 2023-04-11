#include "test_scpi_cmds_in_session.h"
#include "modulemanagerfortest.h"
#include <scpimodule.h>
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <statusmodule.h>
#include <QTest>

QTEST_MAIN(test_scpi_cmds_in_session)

void test_scpi_cmds_in_session::initialSession()
{   // First test on ModuleManagerForTest to know it works as expected
    ModuleManagerForTest modman;

    SCPIMODULE::cSCPIModule scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 1);
    QList<QString> componentList = storageHash->getEntityComponents(entityList[0]);
    QCOMPARE(componentList.count(), 4); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile
}

void test_scpi_cmds_in_session::initialTestClient()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

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

void test_scpi_cmds_in_session::initialScpiCommandsOnOtherModules()
{
    ModuleManagerForTest modman;
    // !!! Pitfall: modules handled by scpimodule must be added first
    // Maybe a bit far fetched but: how about adding modulemanager in
    // zera-classes and use (demo) session files...
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem());
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QCOMPARE(entityList.count(), 2);

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
