#include "test_measmode.h"
#include "measmode.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesvar.h"
#include "measmodecatalogfortest.h"
#include <QTest>

QTEST_MAIN(test_measmode)

void test_measmode::init()
{
    MeasModeCatalogForTest::enableTest();
}

void test_measmode::gettersReportProperCtorParams()
{
    MeasMode mode1("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QCOMPARE(mode1.getName(), "4LW");
    QCOMPARE(mode1.getDspSelectCode(), m4lw);
    QCOMPARE(mode1.getCurrentMask(), "101");

    MeasMode mode2("XLW", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QCOMPARE(mode2.getName(), "XLW");
    QCOMPARE(mode2.getDspSelectCode(), mXlw);
    QCOMPARE(mode2.getCurrentMask(), "110");
}

void test_measmode::fooIsInvalid()
{
    MeasMode mode("foo", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("110"), 3));
    QVERIFY(!mode.isValid());
}

void test_measmode::defaultCtorIsInvalid()
{
    MeasMode mode;
    QVERIFY(!mode.isValid());
}

void test_measmode::validCanChangeMask()
{
    MeasMode mode("XLW", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QVERIFY(mode.tryChangeMask("001"));
    QCOMPARE(mode.getCurrentMask(), "001");
}

void test_measmode::fixedStrategyCannotChangePhase()
{
    MeasMode mode("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QVERIFY(!mode.tryChangeMask("010"));
    QCOMPARE(mode.getCurrentMask(), "101");
}

void test_measmode::invalidForMeasSystemTooLarge()
{
    MeasMode mode("4LW", m4lw, 4, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("101"), 3));
    QCOMPARE(mode.isValid(), false);
}

void test_measmode::invalidForMeasSystemTooSmall()
{
    MeasMode mode("4LW", m4lw, 0, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("101"), 3));
    QCOMPARE(mode.isValid(), false);
}

void test_measmode::invalidReturnsUnsetMask()
{
    MeasMode mode("foo", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QCOMPARE(mode.getCurrentMask(), "000");
}

void test_measmode::invalidCannotChangePhases()
{
    MeasMode mode("foo", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QVERIFY(!mode.tryChangeMask("001"));
}

void test_measmode::tooLongMaskNotAccepted()
{
    MeasMode mode("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("101"), 3));
    QVERIFY(!mode.tryChangeMask("0101"));
    QCOMPARE(mode.getCurrentMask(), "101");
}

void test_measmode::maskWithInvalidCharsNotAccepted()
{
    MeasMode mode("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("101"), 3));
    QVERIFY(!mode.tryChangeMask("0X0"));
    QCOMPARE(mode.getCurrentMask(), "101");
}

void test_measmode::onePhaseMeasSystem()
{
    MeasMode mode("2LW", m2lw, 1, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("1"), 1));
    QCOMPARE(mode.getCurrentMask(), "1");
}

void test_measmode::twoPhaseMeasSystem()
{
    MeasMode mode("2LW", m2lw, 2, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("01"), 1));
    QCOMPARE(mode.getCurrentMask(), "01");
    QVERIFY(mode.tryChangeMask("10"));
    QCOMPARE(mode.getCurrentMask(), "10");
}

void test_measmode::phaseGetter()
{
    MeasMode mode1("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("110"), 3));
    QCOMPARE(mode1.isPhaseActive(0), true);
    QCOMPARE(mode1.isPhaseActive(1), true);
    QCOMPARE(mode1.isPhaseActive(2), false);
    MeasMode mode2("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QCOMPARE(mode2.isPhaseActive(0), true);
    QCOMPARE(mode2.isPhaseActive(1), false);
    QCOMPARE(mode2.isPhaseActive(2), true);
    MeasMode mode3("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3));
    QCOMPARE(mode3.isPhaseActive(-1), false);
    QCOMPARE(mode3.isPhaseActive(4), false);
}
