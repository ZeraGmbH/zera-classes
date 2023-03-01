#include "test_measmodephasesetstrategyphasesvar.h"
#include "measmodephasesetstrategyphasesvar.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyphasesvar)

void test_measmodephasesetstrategyphasesvar::canChangeMask()
{
    MeasModePhaseSetStrategyPhasesVar measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategyphasesvar::phaseInitial()
{
    MeasModePhaseSetStrategyPhasesVar measStrat1(MModePhaseMask("100"));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategyPhasesVar measStrat2(MModePhaseMask("010"));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategyPhasesVar measStrat3(MModePhaseMask("001"));
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
    MeasModePhaseSetStrategyPhasesVar measStrat4(MModePhaseMask("111"));
    QCOMPARE(measStrat4.getCurrPhaseMask(), MModePhaseMask("111"));
    MeasModePhaseSetStrategyPhasesVar measStrat5(MModePhaseMask("000"));
    QCOMPARE(measStrat5.getCurrPhaseMask(), MModePhaseMask("000"));
}

void test_measmodephasesetstrategyphasesvar::phaseChangeValid()
{
    MeasModePhaseSetStrategyPhasesVar measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.tryChangePhase(1));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("010"));
}

void test_measmodephasesetstrategyphasesvar::phaseChangeInvalid()
{
    MeasModePhaseSetStrategyPhasesVar measStrat1(MModePhaseMask("100"));
    QVERIFY(!measStrat1.tryChangePhase(-1));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));

    MeasModePhaseSetStrategyPhasesVar measStrat2(MModePhaseMask("010"));
    QVERIFY(!measStrat2.tryChangePhase(MeasPhaseCount));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
}

void test_measmodephasesetstrategyphasesvar::maskSet()
{
    MeasModePhaseSetStrategyPhasesVar measStrat(MModePhaseMask("000"));
    QVERIFY(measStrat.tryChangeMask(MModePhaseMask("110")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("110"));
}
