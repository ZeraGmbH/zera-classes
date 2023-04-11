#include "test_scpi_cmds_in_session.h"
#include "modulemanagerfortest.h"
#include <scpimodule.h>
#include <scpimodulefortest.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <statusmodule.h>
#include <rangemodule.h>
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

    SCPIMODULE::cSCPIModule scpiModule(1, 9999, modman.getStorageSystem());
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    QCOMPARE(getEntityCount(&modman), 1);
    VeinStorage::VeinHash* storageHash = modman.getStorageSystem();
    QList<int> entityList = storageHash->getEntityList();
    QList<QString> componentList = storageHash->getEntityComponents(entityList[0]);
    QCOMPARE(componentList.count(), 4); // EntitiyName / Metadata / PAR_SerialScpiActive / ACT_SerialScpiDeviceFile
}

void test_scpi_cmds_in_session::initialTestClient()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem());
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
    STATUSMODULE::cStatusModule statusModule(1, 1150, modman.getStorageSystem());
    modman.addModule(&statusModule, QStringLiteral(CONFIG_SOURCES_STATUSMODULE) + "/" + "demo-statusmodule.xml");
    RANGEMODULE::cRangeModule rangeModule(1, 1020, modman.getStorageSystem());
    modman.addModule(&rangeModule, QStringLiteral(CONFIG_SOURCES_RANGEMODULE) + "/" + "demo-rangemodule.xml");
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem());
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
