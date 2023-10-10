#ifndef LAMBDACALCULATOR_H
#define LAMBDACALCULATOR_H

#include <QVector>

struct PhaseSumValues{
    PhaseSumValues();
    QVector<double> phases;
    double sum = 0.0;
};

class LambdaCalculator
{
public:
    static PhaseSumValues calculateAllLambdas(const PhaseSumValues &activePower, const PhaseSumValues &apparentPower, QString measModeActivePower, QString phaseMask);
};

#endif // LAMBDACALCULATOR_H
