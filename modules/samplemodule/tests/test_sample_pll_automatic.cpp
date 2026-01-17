#include "test_sample_pll_automatic.h"
#include "pllautomatic.h"
#include <vf-cpp-entity.h>
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_sample_pll_automatic)

constexpr float epsilon = 1e-6;

constexpr float RangeUMt = 250;
constexpr float UMtLimit = RangeUMt * PllAutomatic::RelativeAmplitudeMinForPllChannelSelection;
constexpr float UMtAbove = UMtLimit + epsilon;
constexpr float UMtBelow = UMtLimit - epsilon;

constexpr float RangeIMt = 10;
constexpr float IMtLimit = RangeIMt * PllAutomatic::RelativeAmplitudeMinForPllChannelSelection;
constexpr float IMtAbove = IMtLimit + epsilon;
constexpr float IMtBelow = IMtLimit - epsilon;

void test_sample_pll_automatic::cleanup()
{
    m_testRunner.reset();
}

void test_sample_pll_automatic::emitImmediateAfterStart()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
}

void test_sample_pll_automatic::switchOffNoEmit()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);

    pllAuto.activate(false);
    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 1);
}

void test_sample_pll_automatic::emitTwiceAfterStart()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0");
    pllAuto.activate(true);
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    fireRangeValues(UMtAbove, QList<int>() << 0);
    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 2);
}

void test_sample_pll_automatic::emitTwiceAfterStartStopAndMoreEmit()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0");
    pllAuto.activate(true);
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    fireRangeValues(UMtAbove, QList<int>() << 0);
    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 2);

    pllAuto.activate(false);

    fireRangeValues(UMtAbove, QList<int>() << 0);
    fireRangeValues(UMtAbove, QList<int>() << 0);
    QCOMPARE(spy.count(), 2);
}

void test_sample_pll_automatic::emitUL2IfUL1IsZero()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    fireRangeValues(UMtAbove, QList<int>() << 1);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m1");
}

constexpr int phaseCountMt = 8;

void test_sample_pll_automatic::emitUL2IfUL1IsBelowAndUl2IsAbove()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(UMtBelow, QList<int>() << 0, dspRangeValues);
    fireRangeValues(UMtAbove, QList<int>() << 1, dspRangeValues);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m1");
}

void test_sample_pll_automatic::emitIL1IfVoltagesAreBelow()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2" << "m3" << "m4"<< "m5");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(UMtBelow, QList<int>() << 0, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 1, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 2, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 3, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 4, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 5, dspRangeValues);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m3");
}

void test_sample_pll_automatic::emitIL2IfLeadingAreBelow()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2" << "m3" << "m4"<< "m5");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(UMtBelow, QList<int>() << 0, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 1, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 2, dspRangeValues);
    fireRangeValues(IMtBelow, QList<int>() << 3, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 4, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 5, dspRangeValues);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m4");
}

void test_sample_pll_automatic::emitSequence()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2" << "m3" << "m4"<< "m5");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(UMtAbove, QList<int>() << 0, dspRangeValues);
    fireRangeValues(UMtAbove, QList<int>() << 1, dspRangeValues);
    fireRangeValues(UMtAbove, QList<int>() << 2, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 3, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 4, dspRangeValues);
    fireRangeValues(IMtAbove, QList<int>() << 5, dspRangeValues);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy[0][0], "m0");

    fireRangeValues(UMtBelow, QList<int>() << 0, dspRangeValues);
    QCOMPARE(spy[1][0], "m1");
    fireRangeValues(UMtBelow, QList<int>() << 1, dspRangeValues);
    QCOMPARE(spy[2][0], "m2");
    fireRangeValues(UMtBelow, QList<int>() << 2, dspRangeValues);
    QCOMPARE(spy[3][0], "m3");
    fireRangeValues(IMtBelow, QList<int>() << 3, dspRangeValues);
    QCOMPARE(spy[4][0], "m4");
    fireRangeValues(IMtBelow, QList<int>() << 4, dspRangeValues);
    QCOMPARE(spy[5][0], "m5");
}

void test_sample_pll_automatic::allBelowDefaultsUL1()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/range-and-sample.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList() << "m0" << "m1"<< "m2" << "m3" << "m4"<< "m5");
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    DemoValuesDspRange dspRangeValues(phaseCountMt);
    fireRangeValues(UMtBelow, QList<int>() << 0, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 1, dspRangeValues);
    fireRangeValues(UMtBelow, QList<int>() << 2, dspRangeValues);
    fireRangeValues(IMtBelow, QList<int>() << 3, dspRangeValues);
    fireRangeValues(IMtBelow, QList<int>() << 4, dspRangeValues);
    fireRangeValues(IMtBelow, QList<int>() << 5, dspRangeValues);
    QCOMPARE(spy.count(), 0);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0], "m0");
}

void test_sample_pll_automatic::emitNothingOnEmptyChannelList()
{
    m_testRunner = std::make_unique<ModuleManagerTestRunner>(":/sessions/sample-only-UL1.json");
    PllAutomatic pllAuto(m_testRunner->getVeinStorageDb(), QStringList());
    QSignalSpy spy(&pllAuto, &PllAutomatic::sigSelectPllChannel);

    pllAuto.activate(true);
    QCOMPARE(spy.count(), 0);
}

void test_sample_pll_automatic::fireRangeValues(float rmsValue, QList<int> activePhaseIdxs)
{
    TestDspInterfacePtr dspInterface = m_testRunner->getDspInterface(RangeModuleEntityId, MODULEPROG);
    DemoValuesDspRange dspRangeValues(phaseCountMt);
    for(int i = 0; i < phaseCountMt; i++)
        if (activePhaseIdxs.contains(i))
            dspRangeValues.setRmsPeakDCValue(i, rmsValue);
    dspInterface->fireActValInterrupt(dspRangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}

void test_sample_pll_automatic::fireRangeValues(float rmsValue, QList<int> activePhaseIdxs, DemoValuesDspRange &dspRangeValues)
{
    TestDspInterfacePtr dspInterface = m_testRunner->getDspInterface(RangeModuleEntityId, MODULEPROG);
    for(int i = 0; i < phaseCountMt; i++)
        if (activePhaseIdxs.contains(i))
            dspRangeValues.setRmsPeakDCValue(i, rmsValue);
    dspInterface->fireActValInterrupt(dspRangeValues.getDspValues(), /* dummy */ 0);
    TimeMachineObject::feedEventLoop();
}
