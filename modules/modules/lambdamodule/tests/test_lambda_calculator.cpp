#include "test_lambda_calculator.h"
#include <QTest>

QTEST_MAIN(test_lambda_calculator)

void test_lambda_calculator::init()
{
    for(double &phasePower : m_activePower.phases)
        phasePower = 1.0;
    m_activePower.sum = 1.0;

    for(double &phasePower : m_apparentPower.phases)
        phasePower = 1.0;
    m_apparentPower.sum = 1.0;
}

void test_lambda_calculator::powerFactorOneAllPhases()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorPointFiveAllPhases()
{
    for(double &phasePower : m_apparentPower.phases)
        phasePower = 2.0;
    m_apparentPower.sum = 2.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.5);
    QCOMPARE(lambdas.phases.at(1), 0.5);
    QCOMPARE(lambdas.phases.at(2), 0.5);
    QCOMPARE(lambdas.sum, 0.5);
}

void test_lambda_calculator::powerFactorPhaseMappingTest()
{
    m_activePower.phases[0] = 0.1;
    m_activePower.phases[1] = 0.2;
    m_activePower.phases[2] = 0.3;
    m_activePower.sum = 0.4;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.1);
    QCOMPARE(lambdas.phases.at(1), 0.2);
    QCOMPARE(lambdas.phases.at(2), 0.3);
    QCOMPARE(lambdas.sum, 0.4);
}

void test_lambda_calculator::powerFactorPhaseMask110()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "110");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 0.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorPhaseMask011()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "011");
    QCOMPARE(lambdas.phases.at(0), 0.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorPhaseMask010()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "010");
    QCOMPARE(lambdas.phases.at(0), 0.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 0.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorPhaseMask000()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "000");
    QCOMPARE(lambdas.phases.at(0), 0.0);
    QCOMPARE(lambdas.phases.at(1), 0.0);
    QCOMPARE(lambdas.phases.at(2), 0.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorPhaseMaskEmpty()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "", "");
    QCOMPARE(lambdas.phases.at(0), 0.0);
    QCOMPARE(lambdas.phases.at(1), 0.0);
    QCOMPARE(lambdas.phases.at(2), 0.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorActivePowerMeasMode4LW()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambda_calculator::powerFactorActivePowerMeasMode3LW()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_apparentPower, "3LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);
}

