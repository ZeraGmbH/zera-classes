#include "test_measmodephasesetstrategy4wire.h"
#include "measmodephasesetstrategy4wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy4wire)

void test_measmodephasesetstrategy4wire::maskInitialValue()
{
    MeasModePhaseSetStrategy4Wire measStrat;
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("111"));
}
