#include "test_range_scpi.h"
#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QTest>

QTEST_MAIN(test_range_scpi)

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
