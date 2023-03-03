#include "test_measmodephasesetstrategy2wirefixedphase.h"
#include "measmodephasesetstrategy2wirefixedphase.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy2wirefixedphase)

void test_measmodephasesetstrategy2wirefixedphase::phaseInitial()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat1(0);
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategy2WireFixedPhase measStrat2(1);
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategy2WireFixedPhase measStrat3(2);
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
    MeasModePhaseSetStrategy2WireFixedPhase measStrat4(-1);
    QCOMPARE(measStrat4.getCurrPhaseMask(), MModePhaseMask("000"));
}

void test_measmodephasesetstrategy2wirefixedphase::measSysCount()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat(0);
    QCOMPARE(measStrat.getActiveMeasSysCount(), 1);
}
