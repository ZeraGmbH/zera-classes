#include "test_measmode.h"
#include "measmode.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesvar.h"
#include "measmodephasesetstrategy2wire.h"
#include "measmodecatalogfortest.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_measmode)

void test_measmode::init()
{
    MeasModeCatalogForTest::enableTest();
}

void test_measmode::gettersReportProperCtorParams()
{
    MeasMode mode1("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QCOMPARE(mode1.getInfo().getName(), "4LW");
    QCOMPARE(mode1.getDspSelectCode(), m4lw);
    QCOMPARE(mode1.getCurrentMask(), "101");

    MeasMode mode2("XLW", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QCOMPARE(mode2.getInfo().getName(), "XLW");
    QCOMPARE(mode2.getDspSelectCode(), mXlw);
    QCOMPARE(mode2.getCurrentMask(), "110");
}

void test_measmode::validPhaseChangeSignal()
{
    MeasMode mode("XLW", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask("001");
    QCOMPARE(mode.getCurrentMask(), "001");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
}

void test_measmode::fixedStrategyCannotChangePhase()
{
    MeasMode mode("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask("010");
    QCOMPARE(mode.getCurrentMask(), "101");
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmode::invalidModeName()
{
    MeasMode mode("foo", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QCOMPARE(mode.isValid(), false);
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

void test_measmode::invalidCannotChangePhases()
{
    MeasMode mode("foo", mXlw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("110"), 3));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask("001");
    QCOMPARE(mode.getCurrentMask(), "110");
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmode::tooLongMaskNotAccepted()
{
    MeasMode mode("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask("0101");
    QCOMPARE(mode.getCurrentMask(), "101");
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmode::onePhaseMeasSystem()
{
    MeasMode mode("2LW", m2lw, 1, std::make_unique<MeasModePhaseSetStrategy2Wire>(MModePhaseMask("1")));
    QCOMPARE(mode.getCurrentMask(), "1");
}

void test_measmode::twoPhaseMeasSystem()
{
    MeasMode mode("2LW", m2lw, 2, std::make_unique<MeasModePhaseSetStrategy2Wire>(MModePhaseMask("01")));
    QCOMPARE(mode.getCurrentMask(), "01");
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask("10");
    QCOMPARE(mode.getCurrentMask(), "10");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
}
