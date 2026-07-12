#include "test_scpi_module_sorted_responses.h"
#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <vf_server_component_setter.h>
#include <vf_cmd_event_handler_item_test.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_module_sorted_responses)

static constexpr int rangeEntityId = 1020;

void test_scpi_module_sorted_responses::initTestCase()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-scpi-and-range.json");
}

void test_scpi_module_sorted_responses::paramQuerySequence()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QStringList scpiResponses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiResponseSorted, this, [&](const QString &scpiResponse) {
        scpiResponses.append(scpiResponse);
    });
    client.sendScpiCmds("CONF:RNG1:GROUPING?\n"
                        "CONF:RNG1:RNGAUTO?\n"
                        "SENS:RNG1:UL1:RANG?\n");

    TimeMachineObject::feedEventLoop();
    QCOMPARE(scpiResponses.size(), 3);
    QCOMPARE(scpiResponses[0], "1");
    QCOMPARE(scpiResponses[1], "0");
    QCOMPARE(scpiResponses[2], "250V");
}

void test_scpi_module_sorted_responses::paramQuerySequenceWithIntermediateServerNotification()
{
    SCPIMODULE::cSCPIModule *scpiModule = qobject_cast<SCPIMODULE::cSCPIModule*>(m_testRunner->getModule(9999));
    SCPIMODULE::ScpiTestClient client(scpiModule);

    QStringList scpiResponses;
    connect(&client, &SCPIMODULE::ScpiTestClient::sigScpiResponseSorted, this, [&](const QString &scpiResponse) {
        scpiResponses.append(scpiResponse);
    });
    client.sendScpiCmds("CONF:RNG1:GROUPING?\n"
                        "CONF:RNG1:RNGAUTO?\n"
                        "SENS:RNG1:UL1:RANG?\n");
    spawnServerNotififcation();

    TimeMachineObject::feedEventLoop();
    QCOMPARE(scpiResponses.size(), 3);
    QCOMPARE(scpiResponses[0], "1");
    QCOMPARE(scpiResponses[1], "0");
    QCOMPARE(scpiResponses[2], "250V");
}

void test_scpi_module_sorted_responses::spawnServerNotififcation()
{
    // Background: Parameter delegate (cSCPIParameterDelegate) creates SCPI response also on server notification
    VfCmdEventHandlerSystemPtr cmdEventHandler = m_testRunner->getVfCmdEventHandlerSystemPtr();
    emit cmdEventHandler->sigSendEvent(VfServerComponentSetter::generateEvent(rangeEntityId, "PAR_Channel1Range", "250V", "250V"));
}
