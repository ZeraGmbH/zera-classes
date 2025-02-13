#include "test_range_scpi.h"
#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <QTest>

QTEST_MAIN(test_range_scpi)

void test_range_scpi::initialStatesQueries()
{
    ModuleManagerTestRunner testRunner(":/session-range-scpi.json");
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:GROUPING?"), "1");
    QCOMPARE(client.sendReceive("CONFIGURATION:RNG1:RNGAUTO?"), "0");
    QCOMPARE(client.sendReceive("SENSE:RNG1:UL1:RANGE:CATALOG?"), "250V;8V;100mV");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IL1:RANGE:CATALOG?"), "10A;5A;2.5A;1.0A;500mA;250mA;100mA;50mA;25mA");
    QCOMPARE(client.sendReceive("SENSE:RNG1:IAUX:RANGE:CATALOG?"), "--");
}
