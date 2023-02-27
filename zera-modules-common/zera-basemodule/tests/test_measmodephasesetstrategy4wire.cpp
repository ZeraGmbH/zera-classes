#include "test_measmodephasesetstrategy4wire.h"
#include "measmodephasesetstrategy4wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy4wire)

void test_measmodephasesetstrategy4wire::canChangeMask()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QVERIFY(!measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategy4wire::phaseChangeDeny()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QVERIFY(!measStrat.tryChangePhase(0));
    QVERIFY(!measStrat.tryChangePhase(1));
    QVERIFY(!measStrat.tryChangePhase(2));
}

void test_measmodephasesetstrategy4wire::maskChangeDeny()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("000")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("010")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("111")));
}

void test_measmodephasesetstrategy4wire::maskInitialValue()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("111"));
}
