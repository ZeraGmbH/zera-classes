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
    TestDspInterfacePtr osciDspInterface = testRunner.getDspInterface(osciEntityId);

    QString measProgramDumped = TestLogHelpers::dump(osciDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

void test_osci_module_regression::dumpDspSetReference()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m0", "m1");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m1", "m2");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m2", "m3");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m3", "m4");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m4", "m5");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m5", "m0");

    TestDspInterfacePtr osciDspInterface = testRunner.getDspInterface(osciEntityId);
    QString measProgramDumped = TestLogHelpers::dump(osciDspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-ref-UL1-to-other.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

void test_osci_module_regression::setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem, QString channelMNameOld, QString channelMNameNew)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(osciEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_RefChannel", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(channelMNameOld, channelMNameNew);
    TimeMachineObject::feedEventLoop();
}
