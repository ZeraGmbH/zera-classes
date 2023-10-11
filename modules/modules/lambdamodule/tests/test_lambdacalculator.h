#ifndef TEST_LAMBDA_CALCULATOR_H
#define TEST_LAMBDA_CALCULATOR_H

#include "lambdacalculator.h"
#include <QObject>

class test_lambdacalculator : public QObject
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
    void powerFactorActivePowerMeasMode2LW();

    void powerFactorPhase1Apparent0();
    void powerFactorPhase3Apparent0();
    void powerFactorApparentSum0();

    void powerFactorLimitedTo1();
private:
    PhaseSumValues m_activePower;
    PhaseSumValues m_apparentPower;
};

#endif // TEST_LAMBDA_CALCULATOR_H
