#include "test_measmodephasesetstrategy2wire.h"
#include "measmodephasesetstrategy2wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy2wire)

void test_measmodephasesetstrategy2wire::phaseInitial()
{
    MeasModePhaseSetStrategy2Wire measStrat;
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("001"));
}

void test_measmodephasesetstrategy2wire::measSysCount()
{
    MeasModePhaseSetStrategy2Wire measStrat;
    QCOMPARE(measStrat.getActiveMeasSysCount(), 1);
}
