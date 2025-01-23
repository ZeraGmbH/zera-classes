#include "test_scpi_xsession_change.h"
#include "modulemanagertestrunner.h"
#include "scpimoduleclientblocked.h"
#include <mocklxdmconfigfilegenerator.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_xsession_change)

void test_scpi_xsession_change::queryXSessionCatalog()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("CONFIGURATION:SYST:XSESSION:CATALOG?"), "Foo;Bar");
}

void test_scpi_xsession_change::queryCurrentXSession()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("CONFIGURATION:SYST:XSESSION?"), "Bar");
}

void test_scpi_xsession_change::setXSession()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-only.json");
    ScpiModuleClientBlocked client;
    QCOMPARE(client.sendReceive("CONFIGURATION:SYST:XSESSION?"), "Bar");
    QCOMPARE(client.sendReceive("CONFIGURATION:SYST:XSESSION Foo;"), "");
    QTest::qWait(1000); // Hmm: FF or waste of time - we have no network mock for telnet yet...
    QCOMPARE(client.sendReceive("CONFIGURATION:SYST:XSESSION?"), "Foo");
}
