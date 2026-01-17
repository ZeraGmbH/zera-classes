#include "test_fft_module_regression.h"
#include "demovaluesdspfft.h"
#include "modulemanagertestrunner.h"
#include <vs_dumpjson.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <timemachineobject.h>
#include <testloghelpers.h>
#include <cmath>
#include <QTest>

QTEST_MAIN(test_fft_module_regression)

static int constexpr fftEntityId = 1060;

void test_fft_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json"); // moving window is off at the time of writing
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorageDb->hasEntity(fftEntityId));
}

void test_fft_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/from-resource.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpInitial.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb,
                                                                   QList<int>() << fftEntityId,
                                                                   QList<int>(),
                                                                   true);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int currentPhaseCount = 4;
static constexpr int fftResultCount = voltagePhaseNeutralCount + currentPhaseCount;

void test_fft_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/sessions/from-resource.json");
    TestDspInterfacePtr fftDspInterface = testRunner.getDspInterface(fftEntityId);

    QStringList valueList = fftDspInterface->getValueList();
    QCOMPARE(valueList.count(), fftResultCount);
}

void test_fft_module_regression::injectValues()
{
    ModuleManagerTestRunner testRunner(":/sessions/from-resource.json");
    TestDspInterfacePtr fftDspInterface = testRunner.getDspInterface(fftEntityId);

    DemoValuesDspFft dspValues(fftDspInterface->getValueList().count());
    dspValues.setValue(0, 0, 1, 1);
    dspValues.setValue(0, 1, 2, 0);
    dspValues.setValue(0, 2, 0, 3);

    dspValues.setValue(1, 0, 3, 5);
    dspValues.setValue(1, 1, 230, 0);
    dspValues.setValue(1, 2, 0, 42);

    dspValues.setValue(2, 0, 5, 5);
    dspValues.setValue(2, 1, 230/M_SQRT2, 230/M_SQRT2);
    dspValues.setValue(2, 2, 42/M_SQRT2, 42/M_SQRT2);

    dspValues.setValue(3, 0, 2, 2);
    dspValues.setValue(3, 1, 4, 0);
    dspValues.setValue(3, 2, 0, 6);

    dspValues.setValue(4, 0, 4, 4);
    dspValues.setValue(4, 1, 6, 0);
    dspValues.setValue(4, 2, 0, 8);

    dspValues.setValue(5, 0, 2, 2);
    dspValues.setValue(5, 1, 4/M_SQRT2, 4/M_SQRT2);
    dspValues.setValue(5, 2, 6/M_SQRT2, 6/M_SQRT2);

    fftDspInterface->fireActValInterrupt(dspValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/dumpActual.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb,
                                                                   QList<int>() << fftEntityId,
                                                                   QList<int>(),
                                                                   true);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(jsonExpected, jsonDumped));
}

void test_fft_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/from-resource.json");
    TestDspInterfacePtr fftDspInterface = testRunner.getDspInterface(fftEntityId);

    QString measProgramDumped = TestLogHelpers::dump(fftDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_fft_module_regression::dumpDspIL1ReferenceSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/ref-IL1.json");
    TestDspInterfacePtr fftDspInterface = testRunner.getDspInterface(fftEntityId);

    QString measProgramDumped = TestLogHelpers::dump(fftDspInterface->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-ref-IL1.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_fft_module_regression::dumpDspSetReference()
{
    ModuleManagerTestRunner testRunner(":/sessions/from-resource.json");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m0", "m1");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m1", "m2");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m2", "m3");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m3", "m4");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m4", "m5");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "m5", "m0");

    TestDspInterfacePtr fftDspInterface = testRunner.getDspInterface(fftEntityId);
    QString measProgramDumped = TestLogHelpers::dump(fftDspInterface->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dump-ref-UL1-to-other.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(measProgramExpected, measProgramDumped));
}

void test_fft_module_regression::setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                                                     QString channelMNameOld,
                                                     QString channelMNameNew)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(fftEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_RefChannel", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(channelMNameOld, channelMNameNew);
    TimeMachineObject::feedEventLoop();
}
