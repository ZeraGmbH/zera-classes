#include "test_lambdacalculator.h"
#include "lambdacalculator.h"
#include <QTest>
#include <cmath>

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
    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactor45DegreesAllPhases()
{
    create45DegreesLoad();

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(2), M_SQRT1_2);
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPointFiveAllPhases()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 2.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.5);
    QCOMPARE(lambdas.phases.at(1), 0.5);
    QCOMPARE(lambdas.phases.at(2), 0.5);
    QCOMPARE(lambdas.sum, 0.5);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPhaseMappingTest()
{
    m_activePower[0] = 0.1;
    m_activePower[1] = 0.2;
    m_activePower[2] = 0.3;
    m_activePower[sumIndex] = 1.2;

    m_reactivePower[1] = -1.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "111");
    QCOMPARE(lambdas.phases.at(0), 0.1);
    QCOMPARE(lambdas.phases.at(1), 0.2);
    QCOMPARE(lambdas.phases.at(2), 0.3);
    QCOMPARE(lambdas.sum, 0.4);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::CapText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::create45DegreesLoad()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = M_SQRT1_2;
    m_activePower[sumIndex] = 3*M_SQRT1_2;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = M_SQRT1_2;
    m_reactivePower[sumIndex] = 3*M_SQRT1_2;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 1.0;
    m_apparentPower[sumIndex] = 3.0;
}

void test_lambdacalculator::powerFactorPhaseMask110()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 2*M_SQRT1_2;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "110");
    QCOMPARE(lambdas.phases.at(0), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPhaseMask011()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 2*M_SQRT1_2;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "011");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(2), M_SQRT1_2);
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPhaseMask010()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 1*M_SQRT1_2;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "010");
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPhaseMask000()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 0.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "000");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 0.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorPhaseMaskEmpty()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 0.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "", "");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 0.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorActivePowerMeasMode4LW()
{
    create45DegreesLoad();

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(2), M_SQRT1_2);
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorActivePowerMeasMode3LW()
{
    m_activePower[sumIndex] = 10;
    m_apparentPower[sumIndex] = 10;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "3LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactor05ActivePowerMeasMode3LW()
{
    create45DegreesLoad();

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "3LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorActivePowerMeasMode2LW()
{
    create45DegreesLoad();
    m_activePower[sumIndex] = 1*M_SQRT1_2;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "2LW", "001");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QVERIFY(qIsNaN(lambdas.phases.at(1)));
    QCOMPARE(lambdas.phases.at(2), M_SQRT1_2);
    QCOMPARE(lambdas.sum, M_SQRT1_2);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::resetAllPowers()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = 0.0;
    m_activePower[sumIndex] = 0.0;
    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = 0.0;
    m_reactivePower[sumIndex] = 0.0;
    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 0.0;
    m_apparentPower[sumIndex] = 0.0;
}


// Unssymmetrical taken from a real load: 40V / 0.5A / angleUI 60°/ -60° / 60°

static constexpr double cos60 = 0.5;
static constexpr double sin60 = 0.866025403784;
static constexpr double apparentPhasePower = 20;

void test_lambdacalculator::genUnsymIndCap(int phase)
{
    resetAllPowers();
    // Power1Module5: 4LS / 111
    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = apparentPhasePower;
    m_apparentPower[sumIndex] = MeasPhaseCount * apparentPhasePower;

    m_activePower[phase] = m_apparentPower[phase] * cos60;
    m_activePower[sumIndex] = m_activePower[phase];

    // Power1Module6: 4LB / 111
    m_reactivePower[0] = m_apparentPower[0] * sin60;
    m_reactivePower[1] = -m_apparentPower[1] * sin60; // that popped up incorrect
    m_reactivePower[2] = m_apparentPower[1] * sin60;
    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[sumIndex] += m_reactivePower[i];
}

void test_lambdacalculator::unsymmetricMeasMode2LWPhase1()
{
    constexpr int phase = 0;
    genUnsymIndCap(phase);

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "2LW", "100");
    QCOMPARE(lambdas.phases[phase], cos60);
    QCOMPARE(lambdas.phaseLoadTypes[phase], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sum, cos60);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::unsymmetricMeasMode2LWPhase2()
{
    constexpr int phase = 1;
    genUnsymIndCap(phase);

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "2LW", "010");
    QCOMPARE(lambdas.phases[phase], cos60);
    QCOMPARE(lambdas.phaseLoadTypes[phase], LambdaCalculator::CapText);
    QCOMPARE(lambdas.sum, cos60);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::CapText);
}

void test_lambdacalculator::unsymmetricMeasMode2LWPhase3()
{
    constexpr int phase = 2;
    genUnsymIndCap(phase);

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "2LW", "001");
    QCOMPARE(lambdas.phases[phase], cos60);
    QCOMPARE(lambdas.phaseLoadTypes[phase], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sum, cos60);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactorPhase1Apparent0()
{
    create45DegreesLoad();
    m_apparentPower[0] = 0.0;
    m_apparentPower[sumIndex] = 2.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QVERIFY(qIsNaN(lambdas.phases.at(0)));
    QCOMPARE(lambdas.phases.at(1), M_SQRT1_2);
    QCOMPARE(lambdas.phases.at(2), M_SQRT1_2);
    // We come in with invalid values:
    // * Apparent power on first phase is 0
    // * Active power on first phase is >0
    // => Limitation forces invalid value to 1
    QCOMPARE(lambdas.sum, 1);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorPhase3Apparent0()
{
    m_apparentPower[2] = 0.0;
    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), 1.0);
    QVERIFY(qIsNaN(lambdas.phases.at(2)));
    QCOMPARE(lambdas.sum, 1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorApparentSum0()
{
    m_activePower[0] = 1.0;
    m_activePower[1] = 0.5;
    m_activePower[2] = 0.5;
    m_apparentPower[0] = 1.0;
    m_apparentPower[1] = -0.5;
    m_apparentPower[2] = -0.5;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), 1.0);
    QCOMPARE(lambdas.phases.at(1), -1.0);
    QCOMPARE(lambdas.phases.at(2), -1.0);
    QVERIFY(qIsNaN(lambdas.sum));

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorLimitedToPlusMinusOne()
{
    m_activePower[0] = -2.0;
    m_activePower[1] = -0.5;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases.at(0), -1.0);
    QCOMPARE(lambdas.phases.at(1), -0.5);
    QCOMPARE(lambdas.phases.at(2), 1.0);
    QCOMPARE(lambdas.sum, 1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactor05Ind()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = 1.0;
    m_activePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = 1.0;
    m_reactivePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 2.0;
    m_apparentPower[sumIndex] = 6.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases[0], 0.5);
    QCOMPARE(lambdas.sum, 0.5);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::IndText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::IndText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::IndText);
}

void test_lambdacalculator::powerFactor05Cap()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = 1.0;
    m_activePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = -1.0;
    m_reactivePower[sumIndex] = -3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 2.0;
    m_apparentPower[sumIndex] = 6.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases[0], 0.5);
    QCOMPARE(lambdas.sum, 0.5);

    QCOMPARE(lambdas.phaseLoadTypes[0], LambdaCalculator::CapText);
    QCOMPARE(lambdas.phaseLoadTypes[1], LambdaCalculator::CapText);
    QCOMPARE(lambdas.phaseLoadTypes[2], LambdaCalculator::CapText);
    QCOMPARE(lambdas.sumLoadType, LambdaCalculator::CapText);
}

void test_lambdacalculator::powerFactor1NoTypeText()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = 1.0;
    m_activePower[sumIndex] = 3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = 0.0;
    m_reactivePower[sumIndex] = 0.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 1.0;
    m_apparentPower[sumIndex] = 3.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases[0], 1.0);
    QCOMPARE(lambdas.sum, 1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}

void test_lambdacalculator::powerFactorMinus1NoTypeText()
{
    for(int i=0; i<MeasPhaseCount; i++)
        m_activePower[i] = -1.0;
    m_activePower[sumIndex] = -3.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_reactivePower[i] = 0.0;
    m_reactivePower[sumIndex] = 0.0;

    for(int i=0; i<MeasPhaseCount; i++)
        m_apparentPower[i] = 1.0;
    m_apparentPower[sumIndex] = 3.0;

    PhaseSumValues lambdas = LambdaCalculator::calculateLambdaValues(m_activePower, m_reactivePower, m_apparentPower, "4LW", "111");
    QCOMPARE(lambdas.phases[0], -1.0);
    QCOMPARE(lambdas.sum, -1.0);

    QCOMPARE(lambdas.phaseLoadTypes[0], "");
    QCOMPARE(lambdas.phaseLoadTypes[1], "");
    QCOMPARE(lambdas.phaseLoadTypes[2], "");
    QCOMPARE(lambdas.sumLoadType, "");
}
