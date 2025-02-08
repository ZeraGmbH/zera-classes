#include "test_dft_module_regression.h"
#include "demovaluesdspdft.h"
#include "modulemanagertestrunner.h"
#include <timemachineobject.h>
#include <vf_core_stack_client.h>
#include <vf_entity_component_event_item.h>
#include <vf_client_component_setter.h>
#include <vs_dumpjson.h>
#include <testloghelpers.h>
#include <QTest>

QTEST_MAIN(test_dft_module_regression)

static int constexpr dftEntityId = 1050;

void test_dft_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/sessions/minimal.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QList<int> entityList = veinStorage->getDb()->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorage->getDb()->hasEntity(dftEntityId));
}

void test_dft_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-no-ref.json");

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpInitial.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

static constexpr int voltagePhaseNeutralCount = 4;
static constexpr int voltagePhasePhaseCount = 3;
static constexpr int currentPhaseCount = 4;
static constexpr int dftResultCount = voltagePhaseNeutralCount + voltagePhasePhaseCount + currentPhaseCount;

void test_dft_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-no-ref.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QStringList valueList = dspInterfaces[0]->getValueList();
    QCOMPARE(valueList.count(), dftResultCount);
}

void test_dft_module_regression::injectActualValuesNoReferenceChannel()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-no-ref.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i;

    dspInterfaces[0]->fireActValInterrupt(actValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpActual-no-ref.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_dft_module_regression::injectActualValuesReferenceChannelUL1()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-ref.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i+1;

    dspInterfaces[0]->fireActValInterrupt(actValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpActual-refUL1.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_dft_module_regression::injectActualValuesReferenceChannelUL2()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-ref.json");
    setReferenceChannel(testRunner.getVfCmdEventHandlerSystemPtr(), "UL1", "UL2"); // Alias (fft/osci expect channel-m-name)

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QVector<float> actValues(dftResultCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<dftResultCount * 2; i++)
        actValues[i] = i+1;

    dspInterfaces[0]->fireActValInterrupt(actValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpActual-refUL2.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));

    // reference is caclulated by dftmodule -> same layout as default
    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll(true));
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpReferenceChannelChange.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

constexpr int comDcRefChannelCount = 6;

void test_dft_module_regression::injectActualValuesOrder0()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-noref-order-0.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QStringList valueList = dspInterfaces[0]->getValueList();
    QCOMPARE(valueList.count(), comDcRefChannelCount);

    QVector<float> actValues(comDcRefChannelCount * 2); // valuelist * 2 for re+im
    for(int i = 0; i<comDcRefChannelCount * 2; i++)
        actValues[i] = i+1;

    dspInterfaces[0]->fireActValInterrupt(actValues, /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpActual-no-ref-order-0.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_dft_module_regression::injectSymmetricalOrder0()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-noref-order-0.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspDft dspValues(dspInterfaces[0]->getValueList(), 0);
    dspValues.setAllValuesSymmetric(20, 10, 30);

    dspInterfaces[0]->fireActValInterrupt(dspValues.getDspValues(), /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpSymmetricOrder0.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_dft_module_regression::injectSymmetricalOrder1()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-ref.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    DemoValuesDspDft dspValues(dspInterfaces[0]->getValueList(), 1);
    dspValues.setAllValuesSymmetric(230, 10, 30);

    dspInterfaces[0]->fireActValInterrupt(dspValues.getDspValues(), /*dummy*/ 0);
    TimeMachineObject::feedEventLoop();

    QByteArray jsonExpected = TestLogHelpers::loadFile(":/veinDumps/dumpSymmetricOrder1.json");
    VeinStorage::AbstractEventSystem* veinStorage = testRunner.getVeinStorageSystem();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorage->getDb(), QList<int>() << dftEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiff(jsonExpected, jsonDumped));
}

void test_dft_module_regression::dumpDspSetup()
{
    ModuleManagerTestRunner testRunner(":/sessions/dft-no-movingwindow-ref.json");

    const QList<TestDspInterfacePtr>& dspInterfaces = testRunner.getDspInterfaceList();
    QCOMPARE(dspInterfaces.count(), 1);

    QString measProgramDumped = TestLogHelpers::dump(dspInterfaces[0]->dumpAll());
    QString measProgramExpected = TestLogHelpers::loadFile(":/dspDumps/dumpMeasProgram.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(measProgramExpected, measProgramDumped));
}

void test_dft_module_regression::setReferenceChannel(VfCmdEventHandlerSystemPtr vfCmdEventHandlerSystem,
                                                     QString channelAliasOld,
                                                     QString channelAliasNew)
{
    VfCmdEventItemEntityPtr entityItem = VfEntityComponentEventItem::create(dftEntityId);
    vfCmdEventHandlerSystem->addItem(entityItem);

    VfClientComponentSetterPtr setter = VfClientComponentSetter::create("PAR_RefChannel", entityItem);
    entityItem->addItem(setter);
    setter->startSetComponent(channelAliasOld, channelAliasNew);
    TimeMachineObject::feedEventLoop();
}

