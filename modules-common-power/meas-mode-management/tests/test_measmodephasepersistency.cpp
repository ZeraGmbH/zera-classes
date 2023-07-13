#include "test_measmodephasepersistency.h"
#include "measmodephasepersistency.h"
#include "measmodephasesetstrategyphasesfixed.h"
#include "measmodephasesetstrategyphasesvar.h"
#include <QTest>

QTEST_MAIN(test_measmodephasepersistency)

void test_measmodephasepersistency::readUnmatchingMode()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QStringList modePhaseList = QStringList() << "XLW,100";
    QVERIFY(!MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, modePhaseList));
}

void test_measmodephasepersistency::readMatchingMode()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QStringList modePhaseList = QStringList() << "2LW,100";
    QVERIFY(MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, modePhaseList));
}

void test_measmodephasepersistency::failOnMissingComma()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QStringList modePhaseList = QStringList() << "2LW";
    QVERIFY(!MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, modePhaseList));
}

void test_measmodephasepersistency::failOnInvalidMask()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QVERIFY(!MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, QStringList() << "2LW,1A0"));
    QVERIFY(!MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, QStringList() << "2LW,10"));
}

void test_measmodephasepersistency::changeOneMaskProperly()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QVERIFY(MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, QStringList() << "2LW,110"));
    QCOMPARE(mode->getCurrentMask(), "110");
}

void test_measmodephasepersistency::changeTwoMasksProperly()
{
    MeasModePtr mode1 = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    MeasModePtr mode2 = std::make_shared<MeasMode>("XLW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesVar>(MModePhaseMask("100"), 3));
    QVERIFY(MeasModePhasePersistency::setMeasModePhaseFromConfig(mode1, QStringList() << "2LW,110" << "XLW,101"));
    QVERIFY(MeasModePhasePersistency::setMeasModePhaseFromConfig(mode2, QStringList() << "2LW,110" << "XLW,101"));
    QCOMPARE(mode1->getCurrentMask(), "110");
    QCOMPARE(mode2->getCurrentMask(), "101");
}

void test_measmodephasepersistency::failOnFixedMask()
{
    MeasModePtr mode = std::make_shared<MeasMode>("2LW", -1, 3, std::make_unique<MeasModePhaseSetStrategyPhasesFixed>(MModePhaseMask("100"), 3));
    QVERIFY(!MeasModePhasePersistency::setMeasModePhaseFromConfig(mode, QStringList() << "2LW,110"));
}
