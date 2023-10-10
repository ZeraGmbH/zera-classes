#ifndef TEST_LAMBDA_CALCULATOR_H
#define TEST_LAMBDA_CALCULATOR_H

#include "lambdacalculator.h"
#include <QObject>

class test_lambda_calculator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void powerFactorOneAllPhases();
    void powerFactorPointFiveAllPhases();
    void powerFactorPhaseMappingTest();

    void powerFactorPhaseMask110();
    void powerFactorPhaseMask011();
    void powerFactorPhaseMask010();
    void powerFactorPhaseMask000();
    void powerFactorPhaseMaskEmpty();

    void powerFactorActivePowerMeasMode4LW();
    void powerFactorActivePowerMeasMode3LW();
private:
    PhaseSumValues m_activePower;
    PhaseSumValues m_apparentPower;
};

#endif // TEST_LAMBDA_CALCULATOR_H
