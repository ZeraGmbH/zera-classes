#include "test_measmodephasesetstrategy3wire.h"
#include "measmodephasesetstrategy3wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy3wire)

void test_measmodephasesetstrategy3wire::canChangeMask()
{
    MeasModePhaseSetStrategy3Wire measStrat;
    QVERIFY(!measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategy3wire::phaseChangeDeny()
{
    MeasModePhaseSetStrategy3Wire measStrat;
    QVERIFY(!measStrat.tryChangePhase(0));
    QVERIFY(!measStrat.tryChangePhase(1));
    QVERIFY(!measStrat.tryChangePhase(2));
}

void test_measmodephasesetstrategy3wire::maskChangeDeny()
{
    MeasModePhaseSetStrategy3Wire measStrat;
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("000")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("010")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("111")));
}

void test_measmodephasesetstrategy3wire::maskInitialValue()
{
    MeasModePhaseSetStrategy3Wire measStrat;
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("101"));
}
