#include "test_measmode.h"
#include "measmode.h"
#include "measmodephasesetstrategyfixedphases.h"
#include "measmodephasesetstrategyxwire.h"
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
    MeasMode mode1("4LW", m4lw, std::make_unique<MeasModePhaseSetStrategyFixedPhases>(MModePhaseMask("101")));
    QCOMPARE(mode1.getInfo()->getName(), "4LW");
    QCOMPARE(mode1.getDspMode(), m4lw);
    QCOMPARE(mode1.getCurrentMask(), MModePhaseMask("101"));

    MeasMode mode2("XLW", mXlw, std::make_unique<MeasModePhaseSetStrategyXWire>(MModePhaseMask("110")));
    QCOMPARE(mode2.getInfo()->getName(), "XLW");
    QCOMPARE(mode2.getDspMode(), mXlw);
    QCOMPARE(mode2.getCurrentMask(), MModePhaseMask("110"));
}

void test_measmode::validPhaseChangeSignal()
{
    MeasMode mode("XLW", mXlw, std::make_unique<MeasModePhaseSetStrategyXWire>(MModePhaseMask("110")));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask(MModePhaseMask("001"));
    QCOMPARE(mode.getCurrentMask(), MModePhaseMask("001"));
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
}

void test_measmode::invalidPhaseChangeSignal()
{
    MeasMode mode("4LW", m4lw, std::make_unique<MeasModePhaseSetStrategyFixedPhases>(MModePhaseMask("101")));
    QSignalSpy spyChanged(&mode, &MeasMode::sigMaskChanged);
    QSignalSpy spyFailed(&mode, &MeasMode::sigMaskChangeFailed);
    mode.tryChangeMask(MModePhaseMask("010"));
    QCOMPARE(mode.getCurrentMask(), MModePhaseMask("101"));
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmode::invalidModeName()
{
    MeasMode mode("foo", m4lw, std::make_unique<MeasModePhaseSetStrategyFixedPhases>(MModePhaseMask("101")));
    QCOMPARE(mode.getInfo(), nullptr);
}
