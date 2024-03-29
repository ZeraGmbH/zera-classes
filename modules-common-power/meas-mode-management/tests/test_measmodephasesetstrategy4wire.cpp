#include "test_measmodephasesetstrategy4wire.h"
#include "measmodephasesetstrategy4wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy4wire)

void test_measmodephasesetstrategy4wire::maskInitialValue()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("111"));
}

void test_measmodephasesetstrategy4wire::measSysCount()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QCOMPARE(measStrat.getActiveMeasSysCount(), 3);
}

void test_measmodephasesetstrategy4wire::maxMeasSysCount()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QCOMPARE(measStrat.getMaxMeasSysCount(), 3);
}
