#include "test_lambdacalculator.h"
#include "lambdacalculator.h"
#include <QTest>

QTEST_MAIN(test_lambdacalculator)

constexpr int sumIndex = MeasPhaseCount;

void test_lambdacalculator::init()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = 1.0;
    m_activePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = 1.0;
    m_reactivePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 1.0;
    m_apparentPower[sumIndex] = 3.0;
}

void test_lambdacalculator::powerFactorOneAllPhases()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPointFiveAllPhases()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 2.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.5);
    QCOMPARE(lambdas.phases.at(1), 0.5);
    QCOMPARE(lambdas.phases.at(2), 0.5);
    QCOMPARE(lambdas.sum, 0.5);
}

void test_lambdacalculator::powerFactorPhaseMappingTest()
{
    m_activePower[0] = 0.1;
    m_activePower[1] = 0.2;
    m_activePower[2] = 0.3;
    m_activePower[sumIndex] = 1.2;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.1);
    QCOMPARE(lambdas.phases.at(1), 0.2);
    QCOMPARE(lambdas.phases.at(2), 0.3);
    QCOMPARE(lambdas.sum, 0.4);
}

void test_lambdacalculator::powerFactorPhaseMask110()
{
    m_activePower[sumIndex] = 2.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "110");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPhaseMask011()
{
    m_activePower[sumIndex] = 2.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "011");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPhaseMask010()
{
    m_activePower[sumIndex] = 1.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "010");
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPhaseMask000()
{
    m_activePower[sumIndex] = 0.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "000");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 0.0);
}

void test_lambdacalculator::powerFactorPhaseMaskEmpty()
{
    m_activePower[sumIndex] = 0.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "", "");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 0.0);
}

void test_lambdacalculator::powerFactorActivePowerMeasMode4LW()
{
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorActivePowerMeasMode3LW()
{
    m_activePower[sumIndex] = 10;
    m_apparentPower[sumIndex] = 20;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "3LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 0.5);
}

void test_lambdacalculator::powerFactorActivePowerMeasMode2LW()
{
    m_activePower[sumIndex] = 1.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "2LW", "001");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPhase1Apparent0()
{
    m_apparentPower[0] = 0.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorPhase3Apparent0()
{
    m_apparentPower[2] = 0.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);
}

void test_lambdacalculator::powerFactorApparentSum0()
{
    m_activePower[0] = 1.0;
    m_activePower[1] = 0.5;
    m_activePower[2] = 0.5;
    m_apparentPower[0] = 1.0;
    m_apparentPower[1] = -0.5;
    m_apparentPower[2] = -0.5;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), -1.0);
    QCOMPARE(lambdas.phases.at(2), -1.0);
    QVERIFY(qIsNaN(lambdas.sum));
}

void test_lambdacalculator::powerFactorLimitedToPlusMinusOne()
{
    m_activePower[0] = -2.0;
    m_activePower[1] = -0.5;

    PhaseSumValues lambdas = LambdaCalculator::calculateAllLambdas(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), -1.0);
    QCOMPARE(lambdas.phases.at(1), -0.5);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);
}
