#include "test_scpi_queue.h"
#include "modulemanagerfortest.h"
#include <scpitestclient.h>
#include <scpiserver.h>
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
