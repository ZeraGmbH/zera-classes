#include "test_measmodephasesetstrategyfixedphases.h"
#include "measmodephasesetstrategyfixedphases.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyfixedphases)

void test_measmodephasesetstrategyfixedphases::canChangeMask()
{
    MeasModePhaseSetStrategyFixedPhases measStrat(MModePhaseMask("111"));
    QVERIFY(!measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategyfixedphases::phaseChangeDeny()
{
    MeasModePhaseSetStrategyFixedPhases measStrat(MModePhaseMask("111"));
    QVERIFY(!measStrat.tryChangePhase(0));
    QVERIFY(!measStrat.tryChangePhase(1));
    QVERIFY(!measStrat.tryChangePhase(2));
}

void test_measmodephasesetstrategyfixedphases::maskChangeDeny()
{
    MeasModePhaseSetStrategyFixedPhases measStrat(MModePhaseMask("111"));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("000")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("010")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("111")));
}

void test_measmodephasesetstrategyfixedphases::maskInitialValue()
{
    MeasModePhaseSetStrategyFixedPhases measStrat1(MModePhaseMask("101"));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("101"));
    MeasModePhaseSetStrategyFixedPhases measStrat2(MModePhaseMask("010"));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
}
