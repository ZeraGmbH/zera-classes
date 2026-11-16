#include "test_range_module_regression.h"
#include "modulemanagertestrunner.h"
#include "demodspvaluesrange.h"
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <testloghelpers.h>
#include <vs_dumpjson.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_range_module_regression)

static int constexpr rangeEntityId = 1020;

void test_range_module_regression::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

void test_range_module_regression::minimalSession()
{
    ModuleManagerTestRunner testRunner(":/session-minimal.json");
    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QList<int> entityList = veinStorageDb->getEntityList();
    QCOMPARE(entityList.count(), 2);
    QVERIFY(veinStorageDb->hasEntity(rangeEntityId));
}

void test_range_module_regression::veinDumpInitial()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");

    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << rangeEntityId);

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/dumpInitial.json", jsonDumped));
}

static constexpr int rangeChannelCount = 8;
static constexpr int rangeRmsValuesCount = 8;
static constexpr int rangePeakValuesCount = 8;
static constexpr int rangeRawPeakValuesCount = 8;
static constexpr int rangeFrequencyCount = 1;
static constexpr int rangeResultCount = rangeRmsValuesCount + rangePeakValuesCount + rangeRawPeakValuesCount + rangeFrequencyCount;

// some tests are huger than wanted to save test time

void test_range_module_regression::checkActualValueCount()
{
    ModuleManagerTestRunner testRunner(":/session-range-test.json");
    TestDspInterfacePtr sampleDspInterfaceProg = testRunner.findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    TestDspInterfacePtr sampleDspInterfaceObser = testRunner.findDspInterfaceByType(INJECT_RANGE_OBSERV);
    TestDspInterfacePtr sampleDspInterfaceAdj = testRunner.findDspInterfaceByType(INJECT_RANGE_ADJUST);

    // dumpDspSetup
    QCOMPARE(sampleDspInterfaceObser->getValueList().size(), rangeChannelCount);
    QCOMPARE(sampleDspInterfaceAdj->getValueList().size(), rangeChannelCount);
    QCOMPARE(sampleDspInterfaceProg->getValueList().size(), rangeChannelCount);

    bool allOk = true;
    // dumping SCALEMEM:GAINCORRECTION2 does not seem reproducable yet
    QString obsermaticDumped = TestLogHelpers::dump(sampleDspInterfaceObser->dumpAll());
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dumpObsermatic.json", obsermaticDumped))
        allOk = false;

    QString adjustListDumped = TestLogHelpers::dump(sampleDspInterfaceAdj->dumpAll(true));
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dumpAdjustManagement.json", adjustListDumped))
        allOk = false;

    QString measProgramDumped = TestLogHelpers::dump(sampleDspInterfaceProg->dumpAll(true));
    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/dspSourceDumps/dumpMeasProgram.json", measProgramDumped))
        allOk = false;


    // injectActualValues:
    DemoDspValuesRange rangeValues(rangeChannelCount);
    rangeValues.setFrequency(15);
    for(int i = 0; i < rangeChannelCount; i++)
        rangeValues.setRmsPeakDCValue(i, i);

    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    VeinStorage::AbstractDatabase *veinStorageDb = testRunner.getVeinStorageDb();
    QByteArray jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << rangeEntityId);

    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/dumpActual.json", jsonDumped))
        allOk = false;


    // injectActualValuesWithPreScaling
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingEnabledGroup0", true);
    testRunner.setVfComponent(rangeEntityId, "PAR_PreScalingGroup0", "2/1*(1)");

    QSignalSpy spyDspVarWrite(sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(500);
    sampleDspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    TestDspVarWriteData ct = spyDspVarWrite.at(0).at(0).value<TestDspVarWriteData>();
    QCOMPARE(spyDspVarWrite.at(0).at(0).value<TestDspVarWriteData>().varName, "GAINCORRECTION");
    QCOMPARE(spyDspVarWrite.at(1).at(0).value<TestDspVarWriteData>().varName, "PHASECORRECTION");
    QCOMPARE(spyDspVarWrite.at(2).at(0).value<TestDspVarWriteData>().varName, "OFFSETCORRECTION");

    jsonDumped = VeinStorage::DumpJson::dumpToByteArray(veinStorageDb, QList<int>() << rangeEntityId);

    if(!TestLogHelpers::compareAndLogOnDiffJsonFile(":/veinDumps/dumpActual-preScaled2.json", jsonDumped))
        allOk = false;
    QVERIFY(allOk);
}
