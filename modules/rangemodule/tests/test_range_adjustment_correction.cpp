#include "test_range_adjustment_correction.h"
#include "testrangehelpers.h"
#include "demodspvaluesrange.h"
#include "testsniffer.h"
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <timemachinefortest.h>
#include <timerfactoryqtfortest.h>
#include <QTest>

QTEST_MAIN(test_range_adjustment_correction)

// The rangemodule runs an adjustment cycle whenever the adjust interval
// (0.5s in the test session) has elapsed AND new actual values arrive!!!!!
// see testRangeAdjTiming() & dump

// Test dumps depend on each other for timeing - we did not find a working
// way to de-couple them

void test_range_adjustment_correction::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
}

// DSP channel numbers as reported by the PCB ('dspchannel' in  mt310s2d.xml)
static int constexpr rangeEntityId = 1020;
static const char* RangeAutomaticComponent("PAR_RangeAutomatic");
static const char* RangeGroupingComponent("PAR_ChannelGrouping");

static constexpr int DspChannelUl1 = 0; // m0
static constexpr int DspChannelIl1 = 1; // m3
static constexpr int DspChannelUl2 = 2; // m1
static constexpr int DspChannelIl2 = 3; // m4

static const char* UL1RangeComponent("PAR_Channel1Range");
static const char* IL1RangeComponent("PAR_Channel4Range");

static constexpr int channelCountInpected = 8;

void test_range_adjustment_correction::init()
{
    TimeMachineForTest::reset();
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/session-range-test.json", /*initialAdjPermission=*/true);
    m_sampleDspInterfaceProg = m_testRunner->findDspInterfaceByType(INJECT_RANGE_PROGRAM);
    m_sampleDspInterfaceObserv = m_testRunner->findDspInterfaceByType(INJECT_RANGE_OBSERV);
    m_sampleDspInterfaceAdj = m_testRunner->findDspInterfaceByType(INJECT_RANGE_ADJUST);

    m_testRunner->setVfComponent(rangeEntityId, RangeGroupingComponent, 0);
    m_testRunner->setVfComponent(rangeEntityId, RangeAutomaticComponent, 0);
    TestRangeHelpers::initCoefficients("m0", "250V");
    TestRangeHelpers::initCoefficients("m4", "10A");
}

void test_range_adjustment_correction::cleanup()
{
    m_testRunner.reset();
    TimeMachineObject::feedEventLoop();
}


void test_range_adjustment_correction::testRangeAdjTiming()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    for (int i=0; i<10; ++i) { // 10*100ms
        TimeMachineForTest::getInstance()->processTimers(101);
        fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
        TimeMachineObject::feedEventLoop();
    }
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/testRangeAdjTiming.json", sniffer.dump(), TEST_SRC_DIR)); // -> correction values @ 500ms & 1000ms
}

void test_range_adjustment_correction::gainCorrectionOrder0()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff0 = 0.98;
    constexpr int order = 0;
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::GAIN, order, coeff0);
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::GAIN, order, coeff0);

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Gain is queried with amplitude rms * preScaling / keeper (= rms here):
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(5.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));
    // Not adjusted channel keeps the default correction (c0 = 1):
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, 1.0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/gainCorrectionOrder0.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::gainCorrectionOrder1()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff1 = 0.001;
    constexpr int order = 1;
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::GAIN, order, coeff1); // UL1 -> 1 + 0.001*x
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::GAIN, order, coeff1); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Setting only c1 keeps the default c0 = 1 of the gain correction:
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, 1.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(5.0, 1.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/gainCorrectionOrder1.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::gainCorrectionOrder2()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff2 = 0.001;
    // Setting only c2 keeps the defaults c0 = 1 and c1 = 0:
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::GAIN, 2, coeff2); // UL1 -> 1 + 0.001*x^2
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::GAIN, 2, coeff2); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, 1.0, 0.0, coeff2), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(5.0, 1.0, 0.0, coeff2), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelIl2));
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, 1.0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "GAINCORRECTION", DspChannelUl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/gainCorrectionOrder2.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::phaseCorrectionOrder0()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff0 = 1.5;
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::PHASE, 0, coeff0); // UL1 -> constant 1.5
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::PHASE, 0, coeff0); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Phase is queried with the signal frequency - both channels get the
    // same correction although their rms values differ:
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/phaseCorrectionOrder0.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::phaseCorrectionOrder1()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff1 = 0.001;
    // Setting only c1 keeps the default c0 = 0 of the phase correction:
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::PHASE, 1, coeff1); // UL1 -> 0.001*f
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::PHASE, 1, coeff1); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, 0.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, 0.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, 0.0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/phaseCorrectionOrder1.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::phaseCorrectionOrder2()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff2 = 0.0001;
    // Setting only c2 keeps the defaults c0 = 0 and c1 = 0:
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::PHASE, 2, coeff2); // UL1 -> 0.0001*f^2
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::PHASE, 2, coeff2); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, 0.0, 0.0, coeff2), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(50.0, 0.0, 0.0, coeff2), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "PHASECORRECTION", DspChannelIl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/phaseCorrectionOrder2.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::offsetCorrectionOrder0()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    constexpr double coeff0 = 0.05;
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::OFFSET, 0, coeff0); // UL1 -> constant 0.05
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::OFFSET, 0, coeff0); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    // Offset is queried with amplitude rms * preScaling and multiplied with
    // the pre-scaling factor before being written (1 here):
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(5.0, coeff0), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelIl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/offsetCorrectionOrder0.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::offsetCorrectionOrder1()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    // A non-constant offset correction does not occur in real adjustment
    // procedures - this just documents what the module would do with it:
    constexpr double coeff1 = 0.001;
    // Setting only c1 keeps the default c0 = 0 of the offset correction:
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::OFFSET, 1, coeff1); // UL1 -> 0.001*x
    TestRangeHelpers::setCorrectionCoefficient("m4", "10A", TestRangeHelpers::OFFSET, 1, coeff1); // IL2

    QSignalSpy spyDspVarWrite(m_sampleDspInterfaceAdj.get(), &TestDspInterface::sigDspVarWrite);
    // m_AdjustTimer -> adjustPrepare() -> m_bAdjustTrigger = true => ActionHandler starts m_adjustMachine -> starts m_writeCorrectionDSPMachine
    TimeMachineForTest::getInstance()->processTimers(501);
    fireActualValues(m_sampleDspInterfaceProg, 100, 5, 50);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(spyDspVarWrite.count(), 3);
    QCOMPARE(TestRangeHelpers::testPolynom(100.0, 0.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelUl1));
    QCOMPARE(TestRangeHelpers::testPolynom(5.0, 0.0, coeff1), TestRangeHelpers::findDspWriteValue(spyDspVarWrite, "OFFSETCORRECTION", DspChannelIl2));

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/offsetCorrectionOrder1.json", sniffer.dump(), TEST_SRC_DIR));
}

void test_range_adjustment_correction::changeRanges()
{
    TestSniffer sniffer;
    sniffer.sniffScpiIoServer(m_testRunner->getPcbServer(), TestSniffer::cmdOnly);
    sniffer.sniffDspVarWrites(m_testRunner->findAllDspInterfaceByEntityId(rangeEntityId), channelCountInpected);
    sniffer.sniffVeinChangeValues(m_testRunner->getVeinStorageDb(), QList<int>() << rangeEntityId);

    sniffer.addComment("Write correction coefficients");
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::GAIN, 0, 0.1);
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::PHASE, 0, 0.2);
    TestRangeHelpers::setCorrectionCoefficient("m0", "250V", TestRangeHelpers::OFFSET, 0, 0.3);

    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::GAIN, 0, 0.4);
    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::PHASE, 0, 0.5);
    TestRangeHelpers::setCorrectionCoefficient("m3", "10A", TestRangeHelpers::OFFSET, 0, 0.6);

    TestRangeHelpers::setCorrectionCoefficient("m0", "8V", TestRangeHelpers::GAIN, 0, 1.1);
    TestRangeHelpers::setCorrectionCoefficient("m0", "8V", TestRangeHelpers::PHASE, 0, 1.2);
    TestRangeHelpers::setCorrectionCoefficient("m0", "8V", TestRangeHelpers::OFFSET, 0, 1.3);

    TestRangeHelpers::setCorrectionCoefficient("m3", "5A", TestRangeHelpers::GAIN, 0, 1.4);
    TestRangeHelpers::setCorrectionCoefficient("m3", "5A", TestRangeHelpers::PHASE, 0, 1.5);
    TestRangeHelpers::setCorrectionCoefficient("m3", "5A", TestRangeHelpers::OFFSET, 0, 1.6);


    sniffer.addComment("Initial");
    TimeMachineForTest::getInstance()->processTimers(501);
    sniffer.addComment("Initial after processTimers");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    sniffer.addComment("Initial after fireActualValues");
    TimeMachineObject::feedEventLoop();
    sniffer.addComment("Initial after feedEventLoop");

    sniffer.addComment("UL1 -> 8V");
    TimeMachineForTest::getInstance()->processTimers(501);
    m_testRunner->setVfComponent(rangeEntityId, UL1RangeComponent, "8V");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    TimeMachineObject::feedEventLoop();

    sniffer.addComment("IL1 -> 5A");
    TimeMachineForTest::getInstance()->processTimers(501);
    m_testRunner->setVfComponent(rangeEntityId, IL1RangeComponent, "5A");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    TimeMachineObject::feedEventLoop();

    sniffer.addComment("UL1 -> 250V");
    TimeMachineForTest::getInstance()->processTimers(501);
    m_testRunner->setVfComponent(rangeEntityId, UL1RangeComponent, "250V");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    TimeMachineObject::feedEventLoop();

    sniffer.addComment("IL1 -> 10A");
    TimeMachineForTest::getInstance()->processTimers(501);
    m_testRunner->setVfComponent(rangeEntityId, IL1RangeComponent, "5A");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    TimeMachineObject::feedEventLoop();

    sniffer.addComment("UL1 -> 8V");
    TimeMachineForTest::getInstance()->processTimers(501);
    m_testRunner->setVfComponent(rangeEntityId, UL1RangeComponent, "8V");
    fireActualValues(m_sampleDspInterfaceProg, 5, 1, 50);
    TimeMachineObject::feedEventLoop();

    QVERIFY(TestLogHelpers::compareAndLogOnDiffJsonSrc("serviceDumps/changeRanges.json", sniffer.dump(), TEST_SRC_DIR));
}


void test_range_adjustment_correction::fireActualValues(const TestDspInterfacePtr &dspInterfaceProg,
                                                        double uRms, double iRms, double frequency)
{
    DemoDspValuesRange rangeValues(8);
    rangeValues.setFrequency(frequency);
    rangeValues.setRmsPeakDCValue(0, uRms); // m0 / UL1
    rangeValues.setRmsPeakDCValue(1, uRms); // m1 / UL2
    rangeValues.setRmsPeakDCValue(2, uRms); // m2 / UL3
    rangeValues.setRmsPeakDCValue(3, iRms); // m3 / IL1
    rangeValues.setRmsPeakDCValue(4, iRms); // m4 / IL2
    rangeValues.setRmsPeakDCValue(5, iRms); // m5 / IL3
    rangeValues.setRmsPeakDCValue(6, uRms); // m6 / UAUX
    rangeValues.setRmsPeakDCValue(7, 0);    // m7 / IAUX
    dspInterfaceProg->fireActValInterrupt(rangeValues.getDspValues(), /* dummy */ 0);
}

