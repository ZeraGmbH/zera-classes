#include "test_measmodephasesetstrategy2wire.h"
#include "measmodephasesetstrategy2wire.h"
#include <QTest>

QTEST_MAIN(test_measmodephasesetstrategy2wire)

void test_measmodephasesetstrategy2wire::canChangeMask()
{
    MeasModePhaseSetStrategy2Wire measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.canChangePhaseMask());
}

void test_measmodephasesetstrategy2wire::phaseInitial()
{
    MeasModePhaseSetStrategy2Wire measStrat1(MModePhaseMask("100"));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("100"));
    MeasModePhaseSetStrategy2Wire measStrat2(MModePhaseMask("010"));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("010"));
    MeasModePhaseSetStrategy2Wire measStrat3(MModePhaseMask("001"));
    QCOMPARE(measStrat3.getCurrPhaseMask(), MModePhaseMask("001"));
}

void test_measmodephasesetstrategy2wire::maskCannotResetAll()
{
    MeasModePhaseSetStrategy2Wire measStrat(MModePhaseMask("100"));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("000")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("100"));
}

void test_measmodephasesetstrategy2wire::maskSetOne()
{
    MeasModePhaseSetStrategy2Wire measStrat1(MModePhaseMask("100"));
    QVERIFY(measStrat1.tryChangeMask(MModePhaseMask("010")));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("010"));

    MeasModePhaseSetStrategy2Wire measStrat2(MModePhaseMask("100"));
    QVERIFY(measStrat2.tryChangeMask(MModePhaseMask("001")));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("001"));
}

void test_measmodephasesetstrategy2wire::maskDenyThree()
{
    MeasModePhaseSetStrategy2Wire measStrat(MModePhaseMask("100"));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("111")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("100"));
}

void test_measmodephasesetstrategy2wire::maskDenyTwoBothCurrentlyNotSet()
{
    MeasModePhaseSetStrategy2Wire measStrat(MModePhaseMask("100"));
    QVERIFY(measStrat.tryChangeMask(MModePhaseMask("100")));
    QVERIFY(!measStrat.tryChangeMask(MModePhaseMask("011")));
    QCOMPARE(measStrat.getCurrPhaseMask(), MModePhaseMask("100"));
}

void test_measmodephasesetstrategy2wire::maskAcceptTwoOneCurrentlySet()
{
    MeasModePhaseSetStrategy2Wire measStrat1(MModePhaseMask("100"));
    QVERIFY(measStrat1.tryChangeMask(MModePhaseMask("100")));
    QVERIFY(measStrat1.tryChangeMask(MModePhaseMask("110")));
    QCOMPARE(measStrat1.getCurrPhaseMask(), MModePhaseMask("010"));

    MeasModePhaseSetStrategy2Wire measStrat2(MModePhaseMask("100"));
    QVERIFY(measStrat2.tryChangeMask(MModePhaseMask("100")));
    QVERIFY(measStrat2.tryChangeMask(MModePhaseMask("101")));
    QCOMPARE(measStrat2.getCurrPhaseMask(), MModePhaseMask("001"));
}
