#include "test_measmodeselector.h"
#include "measmodeselector.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesvar.h"
#include "measmodecatalogfortest.h"
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_measmodeselector)

void test_measmodeselector::init()
{
    MeasModeCatalogForTest::enableTest();
}

void test_measmodeselector::initCurrentModeInvalid()
{
    MeasModeSelector sel;
    QVERIFY(!sel.getCurrMode()->isValid());
}

void test_measmodeselector::initCannotChangeMask()
{
    MeasModeSelector sel;
    QVERIFY(!sel.canChangeMask("000"));
}

void test_measmodeselector::initInactiveMask()
{
    MeasModeSelector sel;
    QCOMPARE(sel.getCurrentMask(), "000");
}

void test_measmodeselector::addCannotChangeMask()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("111"), 3)));
    QVERIFY(!sel.canChangeMask("111"));
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
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("111"), 3)));

    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);
    sel.tryChangeMode("4LW");
    QVERIFY(sel.canChangeMask("110"));
    QCOMPARE(sel.getCurrMode()->getName(), "4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
}

void test_measmodeselector::addValidSelectInvalidMode()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));

    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);
    sel.tryChangeMode("foo");
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyFailed.count(), 1);
}

void test_measmodeselector::addTwoAndSelectThem()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("111"), 3)));
    sel.addMode(std::make_shared<MeasMode>("3LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("101"), 3)));
    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);

    sel.tryChangeMode("4LW");
    QCOMPARE(sel.getCurrMode()->getName(), "4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);

    sel.tryChangeMode("3LW");
    QCOMPARE(sel.getCurrMode()->getName(), "3LW");
    QCOMPARE(spyChanged.count(), 2);
    QCOMPARE(spyFailed.count(), 0);
}

void test_measmodeselector::addAndSelectAndChangeMask()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("111"), 3)));
    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);
    sel.tryChangeMode("4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
    sel.tryChangeMask("110");
    QCOMPARE(spyChanged.count(), 2);
    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(sel.getCurrentMask(), "110");
}

void test_measmodeselector::addAndSelectAndChangeInvalidMask()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("111"), 3)));
    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);
    sel.tryChangeMode("4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
    sel.tryChangeMask("XXX");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 1);
    QCOMPARE(sel.getCurrentMask(), "111");
}

void test_measmodeselector::addTwoSelectThemChangePhases()
{
    MeasModeSelector sel;
    sel.addMode(std::make_shared<MeasMode>("4LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("001"), 3)));
    sel.addMode(std::make_shared<MeasMode>("3LW", m4lw, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("010"), 3)));
    QSignalSpy spyChanged(&sel, &MeasModeSelector::sigTransactionOk);
    QSignalSpy spyFailed(&sel, &MeasModeSelector::sigTransactionFailed);

    sel.tryChangeMode("4LW");
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(spyFailed.count(), 0);
    sel.tryChangeMask("011");
    QCOMPARE(spyChanged.count(), 2);
    QCOMPARE(spyFailed.count(), 0);

    sel.tryChangeMode("3LW");
    QCOMPARE(spyChanged.count(), 3);
    QCOMPARE(spyFailed.count(), 0);
    sel.tryChangeMask("100");
    QCOMPARE(spyChanged.count(), 4);
    QCOMPARE(spyFailed.count(), 0);

    sel.tryChangeMode("4LW");
    QCOMPARE(spyChanged.count(), 5);
    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(sel.getCurrentMask(), "011");

    sel.tryChangeMode("3LW");
    QCOMPARE(spyChanged.count(), 6);
    QCOMPARE(spyFailed.count(), 0);
    QCOMPARE(sel.getCurrentMask(), "100");
}
