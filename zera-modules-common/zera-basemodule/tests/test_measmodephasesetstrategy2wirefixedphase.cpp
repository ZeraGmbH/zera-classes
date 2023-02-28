#include "test_measmodephasesetstrategy2wirefixedphase.h"
#include "measmodephasesetstrategy2wirefixedphase.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy2wirefixedphase)

void test_measmodephasesetstrategy2wirefixedphase::canChangeMask()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat(0);
    QVERIFY(!measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategy2wirefixedphase::phaseInitial()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat1(0);
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategy2WireFixedPhase measStrat2(1);
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategy2WireFixedPhase measStrat3(2);
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
}

void test_measmodephasesetstrategy2wirefixedphase::phaseCannotChange()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat(0);
    QVERIFY(!measStrat.tryChangePhase(1));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("100"));
}

void test_measmodephasesetstrategy2wirefixedphase::maskCannotChange()
{
    MeasModePhaseSetStrategy2WireFixedPhase measStrat1(0);
    QVERIFY(!measStrat1.tryChangeMask(MModePhaseMask("010")));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));

    MeasModePhaseSetStrategy2WireFixedPhase measStrat2(1);
    QVERIFY(!measStrat2.tryChangeMask(MModePhaseMask("001")));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
}
