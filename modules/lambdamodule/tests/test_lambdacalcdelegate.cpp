#include "test_lambdacalcdelegate.h"
#include "lambdacalcdelegate.h"
#include <QTest>

QTEST_MAIN(test_lambdacalcdelegate)

void test_lambdacalcdelegate::measModeAvail3LW()
{
    LambdaCalcDelegate delegate(true, QList<VfModuleActvalue *>(), QList<VfModuleActvalue *>());

    delegate.handleActivePowerChange(0, 1.0);
    delegate.handleActivePowerChange(1, 0.0);
    delegate.handleActivePowerChange(2, 1.0);
    delegate.handleActivePowerChange(3, 2.0);
    delegate.handleApparentPowerChange(0, 1.0);
    delegate.handleApparentPowerChange(1, 2.0);
    delegate.handleApparentPowerChange(2, 1.0);
    delegate.handleApparentPowerChange(3, 8.0);
    delegate.onActivePowerMeasModeChange("3LW");

    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[0]));
    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[1]));
    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[2]));
    QCOMPARE(delegate.getLambdaValues().sum, 0.25);
}

void test_lambdacalcdelegate::measModeNotAvail3LW()
{
    LambdaCalcDelegate delegate(false, QList<VfModuleActvalue *>(), QList<VfModuleActvalue *>());

    delegate.handleActivePowerChange(0, 1.0);
    delegate.handleActivePowerChange(1, 0.0);
    delegate.handleActivePowerChange(2, 1.0);
    delegate.handleActivePowerChange(3, 2.0);
    delegate.handleApparentPowerChange(0, 1.0);
    delegate.handleApparentPowerChange(1, 2.0);
    delegate.handleApparentPowerChange(2, 1.0);
    delegate.handleApparentPowerChange(3, 8.0);
    delegate.onActivePowerMeasModeChange("3LW");

    QCOMPARE(delegate.getLambdaValues().phases[0], 1.0);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.0);
    QCOMPARE(delegate.getLambdaValues().phases[2], 1.0);
    QCOMPARE(delegate.getLambdaValues().sum, 0.5);
}

void test_lambdacalcdelegate::measModeAvailPhaseMask110()
{
    LambdaCalcDelegate delegate(true, QList<VfModuleActvalue *>(), QList<VfModuleActvalue *>());

    delegate.handleActivePowerChange(0, 1.0);
    delegate.handleActivePowerChange(1, 1.0);
    delegate.handleActivePowerChange(2, 0.0);
    delegate.handleActivePowerChange(3, 2.0);
    delegate.handleApparentPowerChange(0, 2.0);
    delegate.handleApparentPowerChange(1, 2.0);
    delegate.handleApparentPowerChange(2, 4.0);
    delegate.handleApparentPowerChange(3, 8.0);
    delegate.onActivePowerPhaseMaskChange("110");

    QCOMPARE(delegate.getLambdaValues().phases[0], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.5);
    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[2]));
    QCOMPARE(delegate.getLambdaValues().sum, 0.5);
}

void test_lambdacalcdelegate::measModeNotAvailPhaseMask110()
{
    LambdaCalcDelegate delegate(false, QList<VfModuleActvalue *>(), QList<VfModuleActvalue *>());

    delegate.handleActivePowerChange(0, 1.0);
    delegate.handleActivePowerChange(1, 1.0);
    delegate.handleActivePowerChange(2, 0.0);
    delegate.handleActivePowerChange(3, 2.0);
    delegate.handleApparentPowerChange(0, 2.0);
    delegate.handleApparentPowerChange(1, 2.0);
    delegate.handleApparentPowerChange(2, 4.0);
    delegate.handleApparentPowerChange(3, 10.0);
    delegate.onActivePowerPhaseMaskChange("110");

    QCOMPARE(delegate.getLambdaValues().phases[0], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[2], 0.0);
    QCOMPARE(delegate.getLambdaValues().sum, 0.25);
}
