#include "test_adj_module_phase.h"
#include "modulemanagertestrunner.h"
#include <scpimoduleclientblocked.h>
#include <QTest>

QTEST_MAIN(test_adj_module_phase)

void test_adj_module_phase::noActValuesWithPermission()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    // Init is missing by intention
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+4");
}

void test_adj_module_phase::validActValuesWithPermission()
{

}

void test_adj_module_phase::noActValuesWithoutPermission()
{
    /*    ModuleManagerTestRunner testRunner(":/session-minimal.json", true);

    // Init is missing by intention
    ScpiModuleClientBlocked scpiClient;
    QString response = scpiClient.sendReceive("calc:adj1:phas UL2,250V,119.9894;|*stb?");
    QCOMPARE(response, "+0");*/
}
