#ifndef TEST_LAMBDA_CALCULATOR_H
#define TEST_LAMBDA_CALCULATOR_H

#include "measmodeinfo.h"
#include <QObject>

class test_lambdacalculator : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void powerFactorOneAllPhases();
    void powerFactor45DegreesAllPhases();
    void powerFactorPointFiveAllPhases();
    void powerFactorPhaseMappingTest();

    void powerFactorPhaseMask110();
    void powerFactorPhaseMask011();
    void powerFactorPhaseMask010();
    void powerFactorPhaseMask000();
    void powerFactorPhaseMaskEmpty();

    void powerFactorActivePowerMeasMode4LW();
    void powerFactorActivePowerMeasMode3LW();
    void powerFactor05ActivePowerMeasMode3LW();
    void powerFactorActivePowerMeasMode2LW();

    void powerFactorPhase1Apparent0();
    void powerFactorPhase3Apparent0();
    void powerFactorApparentSum0();

    void powerFactorLimitedToPlusMinusOne();

    // load type only tests
    void powerFactor05Ind();
    void powerFactor05Cap();
    void powerFactor1NoTypeText();
    void powerFactorMinus1NoTypeText();
private:
    void create45DegreesLoad();

    QVector<double> m_activePower = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);
    QVector<double> m_reactivePower = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);
    QVector<double> m_apparentPower = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);
};

#endif // TEST_LAMBDA_CALCULATOR_H
