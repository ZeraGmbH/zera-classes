#include "test_measmodephasesetstrategyxwire.h"
#include "measmodephasesetstrategyxwire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyxwire)

void test_measmodephasesetstrategyxwire::canChangeMask()
{
    MeasModePhaseSetStrategyXWire measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategyxwire::phaseInitial()
{
    MeasModePhaseSetStrategyXWire measStrat1(MModePhaseMask("100"));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategyXWire measStrat2(MModePhaseMask("010"));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategyXWire measStrat3(MModePhaseMask("001"));
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
    MeasModePhaseSetStrategyXWire measStrat4(MModePhaseMask("111"));
    QCOMPARE(measStrat4.getCurrPhaseMask(), MModePhaseMask("111"));
    MeasModePhaseSetStrategyXWire measStrat5(MModePhaseMask("000"));
    QCOMPARE(measStrat5.getCurrPhaseMask(), MModePhaseMask("000"));
}

void test_measmodephasesetstrategyxwire::phaseChangeValid()
{
    MeasModePhaseSetStrategyXWire measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.tryChangePhase(1));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("010"));
}

void test_measmodephasesetstrategyxwire::phaseChangeInvalid()
{
    MeasModePhaseSetStrategyXWire measStrat1(MModePhaseMask("100"));
    QVERIFY(!measStrat1.tryChangePhase(-1));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));

    MeasModePhaseSetStrategyXWire measStrat2(MModePhaseMask("010"));
    QVERIFY(!measStrat2.tryChangePhase(MeasPhaseCount));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
}

void test_measmodephasesetstrategyxwire::maskSet()
{
    MeasModePhaseSetStrategyXWire measStrat(MModePhaseMask("000"));
    QVERIFY(measStrat.tryChangeMask(MModePhaseMask("110")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("110"));
}
