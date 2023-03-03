#include "test_measmodeselector.h"
#include "measmodeselector.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodecatalogfortest.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_measmodeselector)

void test_measmodeselector::init()
{
    MeasModeCatalogForTest::enableTest();
}

void test_measmodeselector::initNoCurrentMode()
{
    MeasModeSelector sel;
    QCOMPARE(sel.getCurrMode(), nullptr);
}

void test_measmodeselector::invalidNotAdded()
{
    MeasModeSelector sel;
    bool added = sel.addMode(std::make_shared<MeasMode>("foo", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    QVERIFY(!added);
}

void test_measmodeselector::doubleNotAdded()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    bool added = sel.addMode(std::make_shared<MeasMode>("4LW", 3, m4lw, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    QVERIFY(!added);
}

void test_measmodeselector::addAndSelectValidMode()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));

    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigModeChanged);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigModeChangeFailed);
    sel.tryChangeMode("4LW");
    QCOMPARE(sel.getCurrMode()->getInfo().getName(), "4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
}

void test_measmodeselector::addValidSelectInvalidMode()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));

    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigModeChanged);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigModeChangeFailed);
    sel.tryChangeMode("foo");
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmodeselector::addTwoAndSelectThem()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    sel.addMode(std::make_shared<MeasMode>("3LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigModeChanged);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigModeChangeFailed);

    sel.tryChangeMode("4LW");
    QCOMPARE(sel.getCurrMode()->getInfo().getName(), "4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);

    sel.tryChangeMode("3LW");
    QCOMPARE(sel.getCurrMode()->getInfo().getName(), "3LW");
    QCOMPARE(spyChanged.count(), 2);
    QCOMPARE(spyFailed.count(), 0);
}
