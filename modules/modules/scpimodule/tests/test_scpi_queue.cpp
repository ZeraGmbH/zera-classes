#include "test_scpi_queue.h"
#include "modulemanagerfortest.h"
#include <rangemodule.h>
#include <scpitestclient.h>
#include <scpiserver.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <scpimodulefortest.h>
#include <QTest>

QTEST_MAIN(test_scpi_queue)

void enableQueuing(SCPIMODULE::cSCPIInterface* interface)
{
    interface->setEnableQueue(true);
}

void disableQueuing(SCPIMODULE::cSCPIInterface* interface)
{
    interface->setEnableQueue(false);
}

void test_scpi_queue::sendStandardCmdsQueueEnabled()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    enableQueuing(client.getScpiInterface());

    client.sendScpiCmds("*OPC?|*OPC|*stb?");

    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+1");
    QCOMPARE(responses[1], "+0");
}

void test_scpi_queue::sendOneStandardCmdsQueueDisabledAndEnabled()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    disableQueuing(client.getScpiInterface());
    client.sendScpiCmds("*SRE 2;|*SRE?|*IDN?");
    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+2");
    QVERIFY(responses[1].contains("DEMO"));

    responses.clear();
    enableQueuing(client.getScpiInterface());
    client.sendScpiCmds("*ESE?|*RST|*ESR?");
    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+0");
    QCOMPARE(responses[1], "+0");
}

void test_scpi_queue::sendErroneousAndCorrectStandardCmds()
{
    ModuleManagerForTest modman;
    SCPIMODULE::ScpiModuleForTest scpiModule(1, 9999, modman.getStorageSystem(), true);
    modman.addModule(&scpiModule, QStringLiteral(CONFIG_SOURCES_SCPIMODULE) + "/" + "demo-scpimodule.xml");

    SCPIMODULE::ScpiTestClient client(&scpiModule, *scpiModule.getConfigData(), scpiModule.getScpiInterface());
    scpiModule.getSCPIServer()->appendClient(&client);

    QStringList responses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiAnswer, &client, [&responses] (QString response) {
        responses.append(response);
    });

    enableQueuing(client.getScpiInterface());
    client.sendScpiCmds("*CLS?|*OPC?|*IDN|*TST|*stb?");

    QCOMPARE(responses.count(), 2);
    QCOMPARE(responses[0], "+1");
    QCOMPARE(responses[1], "+4");
}

