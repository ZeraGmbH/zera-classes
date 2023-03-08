#include "test_measmodephasesetstrategyxwire.h"
#include "measmodephasesetstrategyxwire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategyxwire)

void test_measmodephasesetstrategyxwire::activePhasesAfterInit()
{
    MeasModePhaseSetStrategyXWire strat1(MModePhaseMask("111"), 3);
    QCOMPARE(strat1.getActiveMeasSysCount(), 3);

    MeasModePhaseSetStrategyXWire strat2(MModePhaseMask("110"), 3);
    QCOMPARE(strat2.getActiveMeasSysCount(), 2);

    MeasModePhaseSetStrategyXWire strat3(MModePhaseMask("100"), 3);
    QCOMPARE(strat3.getActiveMeasSysCount(), 1);


    MeasModePhaseSetStrategyXWire strat4(MModePhaseMask("11"), 2);
    QCOMPARE(strat4.getActiveMeasSysCount(), 2);

    MeasModePhaseSetStrategyXWire strat5(MModePhaseMask("01"), 2);
    QCOMPARE(strat5.getActiveMeasSysCount(), 1);
}

void test_measmodephasesetstrategyxwire::activePhasesAfterChange()
{
    MeasModePhaseSetStrategyXWire strat(MModePhaseMask("111"), 3);
    QVERIFY(strat.tryChangeMask(MModePhaseMask("011")));
    QCOMPARE(strat.getActiveMeasSysCount(), 2);
}
