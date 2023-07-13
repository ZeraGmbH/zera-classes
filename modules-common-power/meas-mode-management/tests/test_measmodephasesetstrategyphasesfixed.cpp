#include "test_measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyphasesfixed)

void test_measmodephasesetstrategyphasesfixed::canChangeMask()
{
    MeasModePhaseSetStrategyPhasesFixed measStrat(MModePhaseMask("111"), 3);
    QVERIFY(!measStrat.isVarMask());
}

void test_measmodephasesetstrategyphasesfixed::maskChangeDeny()
{
    MeasModePhaseSetStrategyPhasesFixed measStrat(MModePhaseMask("111"), 3);
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("000")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("010")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("111")));
}

void test_measmodephasesetstrategyphasesfixed::maskInitialValue()
{
    MeasModePhaseSetStrategyPhasesFixed measStrat1(MModePhaseMask("101"), 3);
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("101"));
    MeasModePhaseSetStrategyPhasesFixed measStrat2(MModePhaseMask("010"), 3);
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
}
