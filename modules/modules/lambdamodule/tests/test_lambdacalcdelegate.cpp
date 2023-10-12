#include "test_lambdacalcdelegate.h"
#include "lambdacalcdelegate.h"
#include <QTest>

QTEST_MAIN(test_lambdacalcdelegate)

void test_lambdacalcdelegate::measModeAvail3LW()
{
    LambdaCalcDelegate delegate(true, QList<VfModuleActvalue *>());

    delegate.onActivePower1Change(1.0);
    delegate.onActivePower2Change(0.0);
    delegate.onActivePower3Change(1.0);
    delegate.onActivePowerSumChange(2.0);
    delegate.onApparentPower1Change(1.0);
    delegate.onApparentPower2Change(2.0);
    delegate.onApparentPower3Change(1.0);
    delegate.onApparentPowerSumChange(8.0);
    delegate.onActivePowerMeasModeChange("3LW");

    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[0]));
    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[1]));
    QVERIFY(qIsNaN(delegate.getLambdaValues().phases[2]));
    QCOMPARE(delegate.getLambdaValues().sum, 0.25);
}

void test_lambdacalcdelegate::measModeNotAvail3LW()
{
    LambdaCalcDelegate delegate(false, QList<VfModuleActvalue *>());

    delegate.onActivePower1Change(1.0);
    delegate.onActivePower2Change(0.0);
    delegate.onActivePower3Change(1.0);
    delegate.onActivePowerSumChange(2.0);
    delegate.onApparentPower1Change(1.0);
    delegate.onApparentPower2Change(2.0);
    delegate.onApparentPower3Change(1.0);
    delegate.onApparentPowerSumChange(8.0);
    delegate.onActivePowerMeasModeChange("3LW");

    QCOMPARE(delegate.getLambdaValues().phases[0], 1.0);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.0);
    QCOMPARE(delegate.getLambdaValues().phases[2], 1.0);
    QCOMPARE(delegate.getLambdaValues().sum, 0.5);//old computations would produce 'sum(p)/sum(s)=0.25'
}

void test_lambdacalcdelegate::measModeAvailPhaseMask110()
{
    LambdaCalcDelegate delegate(true, QList<VfModuleActvalue *>());

    delegate.onActivePower1Change(1.0);
    delegate.onActivePower2Change(1.0);
    delegate.onActivePower3Change(0.0);
    delegate.onActivePowerSumChange(2.0);
    delegate.onApparentPower1Change(2.0);
    delegate.onApparentPower2Change(2.0);
    delegate.onApparentPower3Change(4.0);
    delegate.onApparentPowerSumChange(8.0);
    delegate.onActivePowerPhaseMaskChange("110");

    QCOMPARE(delegate.getLambdaValues().phases[0], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[2], 0.0);
    QCOMPARE(delegate.getLambdaValues().sum, 0.5);
}

void test_lambdacalcdelegate::measModeNotAvailPhaseMask110()
{
    LambdaCalcDelegate delegate(false, QList<VfModuleActvalue *>());

    delegate.onActivePower1Change(1.0);
    delegate.onActivePower2Change(1.0);
    delegate.onActivePower3Change(0.0);
    delegate.onActivePowerSumChange(2.0);
    delegate.onApparentPower1Change(2.0);
    delegate.onApparentPower2Change(2.0);
    delegate.onApparentPower3Change(4.0);
    delegate.onApparentPowerSumChange(10.0);
    delegate.onActivePowerPhaseMaskChange("110");

    QCOMPARE(delegate.getLambdaValues().phases[0], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[1], 0.5);
    QCOMPARE(delegate.getLambdaValues().phases[2], 0.0);
    QCOMPARE(delegate.getLambdaValues().sum, 0.25);//old computations would produce 'sum(p)/sum(s)=0.20'
}
