#include "test_range_scpi.h"
#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include "scpimodulefortest.h"
#include "scpitestclient.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_range_scpi)

constexpr int rangeChangeDelay = 100;

void test_range_scpi::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_range_scpi::scpiQueryAndCommand()
{
    // We put all in here to save time - starting module-manager takes
    // significant amount of time and if one fails trouble is near anyway...
    ModuleManagerTestRunner testRunner(":/session-range-scpi.json");
    ScpiModuleClientBlocked client;

    // initial state queries
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING?"), "1");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:RNGAUTO?"), "0");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE:CATALOG?"), "250V;8V;100mV");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE:CATALOG?"), "10A;5A;2.5A;1.0A;500mA;250mA;100mA;50mA;25mA");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IAUX:RANGE:CATALOG?"), "--");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE?"), "250V");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE?"), "10A");
    QCOMPARE(client.sendReceive("SENSE:RNG1:OVERLOAD?"), "0");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:ENABLEIGNORERMSVAL?"), "0");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:UL1:INVERTPHASE?"), "0");

    // set valid ranges currently active again
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE 250V;|*OPC?"), "+1");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE 10A;|*OPC?"), "+1");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE 250V;|*STB?"), "+0");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE 10A;|*STB?"), "+0");

    // set valid grouping / automatic again
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING 1;|*OPC?"), "+1");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:RNGAUTO 0;|*OPC?"), "+1");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING 1;|*STB?"), "+0");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:RNGAUTO 0;|*STB?"), "+0");

    // change to valid ranges
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE 8V;|*OPC?"), "+1");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE 5A;|*OPC?"), "+1");

    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE 8V;|*STB?"), "+0");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE 5A;|*STB?"), "+0");

    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE 8V;|*OPC?|*STB?"), "+1+0"); // Hmm where is \n?
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE 5A;|*OPC?|*STB?"), "+1+0");

    // set invalid ranges
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE FOO;|*STB?"), "+4");
    QCOMPARE(client.sendReceive("*CLS|*STB?"), "+0");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE BAR;|*STB?"), "+4");
    QCOMPARE(client.sendReceive("*CLS|*STB?"), "+0");
}

void test_range_scpi::rangeChangeWithDelay()
{
    ModuleManagerTestRunner testRunner(":/session-range-scpi.json");
    testRunner.setRangeGetSetDelay(rangeChangeDelay);

    SCPIMODULE::ScpiModuleForTest *scpiModule = static_cast<SCPIMODULE::ScpiModuleForTest*>(testRunner.getModule("scpimodule", 9999));
    QVERIFY(scpiModule != nullptr);
    SCPIMODULE::ScpiTestClient scpiClient(scpiModule, *scpiModule->getConfigData(), scpiModule->getScpiInterface());
    scpiClient.sendScpiCmds("CONFIGURATION:RNG1:GROUPING 0;");
    QSignalSpy spyScpiAnswer(&scpiClient, &SCPIMODULE::ScpiTestClient::sigScpiAnswer);
    QSignalSpy spyCmdProcessed(&scpiClient, &SCPIMODULE::ScpiTestClient::commandAnswered);


    //set existing range again
    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[0][0], "250V");

    spyCmdProcessed.clear();
    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 250V;");
    TimeMachineObject::feedEventLoop(); //No delay needed, this request doesn't reach services, range-module handles it
    QCOMPARE(spyCmdProcessed.count(), 1);

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[1][0], "250V");


    //set a range different than existing
    spyScpiAnswer.clear();
    spyCmdProcessed.clear();

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 8V;");
    TimeMachineForTest::getInstance()->processTimers(rangeChangeDelay);
    QCOMPARE(spyCmdProcessed.count(), 1);

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[0][0], "8V");


    //set range different than existing twice, back to back same request
    spyScpiAnswer.clear();
    spyCmdProcessed.clear();

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 250V;");
    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 250V;");
    TimeMachineObject::feedEventLoop(); //The 2nd request doesn't reach services, range-module handles it with dummy notification
    QCOMPARE(spyCmdProcessed.count(), 1);
    TimeMachineForTest::getInstance()->processTimers(rangeChangeDelay);
    QCOMPARE(spyCmdProcessed.count(), 2);

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[0][0], "250V");


    //1st request: Change to a different range
    //2nd request: Change to a different range (while request 1 is not yet processed)
    spyScpiAnswer.clear();
    spyCmdProcessed.clear();

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 8V;");
    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 100mV;");
    TimeMachineForTest::getInstance()->processTimers(rangeChangeDelay);
    QCOMPARE(spyCmdProcessed.count(), 1);
    TimeMachineForTest::getInstance()->processTimers(rangeChangeDelay);
    QCOMPARE(spyCmdProcessed.count(), 2);

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[0][0], "100mV");


    //1st request: Change to a different range
    //2nd request: Change to an existing range (while request 1 is not yet processed)
    spyScpiAnswer.clear();
    spyCmdProcessed.clear();

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 250V;");
    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE 100mV;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyCmdProcessed.count(), 1); //2nd request handled by range-module, no delay
    TimeMachineForTest::getInstance()->processTimers(rangeChangeDelay);
    QCOMPARE(spyCmdProcessed.count(), 2); //1st request sent to services, response with delay

    scpiClient.sendScpiCmds("SENSE:RNG1:UL1:RANGE?");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(spyScpiAnswer[0][0], "250V");
}
