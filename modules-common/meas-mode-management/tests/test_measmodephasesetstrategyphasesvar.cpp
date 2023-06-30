#include "test_measmodephasesetstrategyphasesvar.h"
#include "measmodephasesetstrategyphasesvar.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyphasesvar)

void test_measmodephasesetstrategyphasesvar::canChangeMask()
{
    MeasModePhaseSetStrategyPhasesVar measStrat(MModePhaseMask("100"), 3);
    QVERIFY(measStrat.isVarMask());
}

void test_measmodephasesetstrategyphasesvar::phaseInitial()
{
    MeasModePhaseSetStrategyPhasesVar measStrat1(MModePhaseMask("100"), 3);
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategyPhasesVar measStrat2(MModePhaseMask("010"), 3);
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategyPhasesVar measStrat3(MModePhaseMask("001"), 3);
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
    MeasModePhaseSetStrategyPhasesVar measStrat4(MModePhaseMask("111"), 3);
    QCOMPARE(measStrat4.getCurrPhaseMask(), MModePhaseMask("111"));
    MeasModePhaseSetStrategyPhasesVar measStrat5(MModePhaseMask("000"), 3);
    QCOMPARE(measStrat5.getCurrPhaseMask(), MModePhaseMask("000"));
}

void test_measmodephasesetstrategyphasesvar::maskSet()
{
    MeasModePhaseSetStrategyPhasesVar measStrat(MModePhaseMask("000"), 3);
    QVERIFY(measStrat.tryChangeMask(MModePhaseMask("110")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("110"));
}
