#include "test_scpi_learn_measure_objects_and_relations.h"
#include "modulemanagertestrunner.h"
#include "scpimodule.h"
#include "scpitestclient.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_scpi_learn_measure_objects_and_relations)

void test_scpi_learn_measure_objects_and_relations::queryMeasure()
{
    ModuleManagerTestRunner testRunner(":/session-scpi-osci-for-min-measure.json");
    SCPIMODULE::cSCPIModule* scpiModule = static_cast<SCPIMODULE::cSCPIModule*>(testRunner.getModule(9999));

    constexpr int measureComponentCount = 8; // ACT_OSCI1..ACT_OSCI18
    QCOMPARE(SCPIMODULE::cSCPIMeasure::getInstanceCount(), measureComponentCount);
    QCOMPARE(SCPIMODULE::ScpiBaseDelegate::getInstanceCount(), 83);


    SCPIMODULE::ScpiTestClient client(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
    client.sendScpiCmds("MEAS?");
    TimeMachineObject::feedEventLoop();
    // client still connected

    QCOMPARE(SCPIMODULE::cSCPIMeasure::getInstanceCount(), 2 * measureComponentCount); // module + client
    QCOMPARE(SCPIMODULE::ScpiBaseDelegate::getInstanceCount(), 133); // ???
}
