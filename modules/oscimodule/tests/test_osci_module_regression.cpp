#include "test_osci_module_regression.h"
#include "modulemanagertestrunner.h"
#include <testloghelpers.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_osci_module_regression)

static int constexpr osciEntityId = 1120;

void test_osci_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    TestDspInterfacePtr osciDspInterface = testRunner.findDspInterfaceByEntityId(osciEntityId);

    QString measProgramDumped = TestLogHelpers::dump(osciDspInterface->dumpAll());
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dumpMeasProgram.json", measProgramDumped));
}

void test_osci_module_regression::dumpDspSetReference()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    setReferenceChannel(testRunner, "m1");
    setReferenceChannel(testRunner, "m2");
    setReferenceChannel(testRunner, "m3");
    setReferenceChannel(testRunner, "m4");
    setReferenceChannel(testRunner, "m5");
    setReferenceChannel(testRunner, "m0");

    TestDspInterfacePtr osciDspInterface = testRunner.findDspInterfaceByEntityId(osciEntityId);
    QString measProgramDumped = TestLogHelpers::dump(osciDspInterface->dumpAll(true));
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dump-ref-UL1-to-other.json", measProgramDumped));
}

void test_osci_module_regression::setReferenceChannel(ModuleManagerTestRunner &modman, const QString &channelAlias)
{
    modman.setVfComponent(osciEntityId, "PAR_RefChannel", channelAlias);
}
