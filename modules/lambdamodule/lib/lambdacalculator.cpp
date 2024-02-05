#include "lambdacalculator.h"
#include "measmodecatalog.h"
#include <math.h>

PhaseSumValues::PhaseSumValues() :
    phases(MeasPhaseCount, 0.0)
{
}

constexpr int sumIndex = MeasPhaseCount;

PhaseSumValues LambdaCalculator::calculateAllLambdas(const QVector<double> &activePower,
                                                     const QVector<double> &reactivePower,
                                                     const QVector<double> &apparentPower,
                                                     QString measModeActivePower,
                                                     QString phaseMaskActivePower)
{
    PhaseSumValues lambdas;
    cMeasModeInfo info = MeasModeCatalog::getInfo(measModeActivePower);
    double apparentPowerSum = 0.0;

    if (info.isThreeWire())
        lambdas = lambdaFor3LW(activePower[sumIndex], apparentPower[sumIndex]);
    else {
        for(int i = 0; i < MeasPhaseCount; i++) {
            if (phaseMaskActivePower.size() > i && phaseMaskActivePower.at(i) == "1") {
                if (abs(apparentPower[i]) < 1e-10)
                    lambdas.phases[i] = qQNaN();
                else {
                    lambdas.phases[i] = limitValueToPlusMinusOne(activePower[i] / apparentPower[i]);
                    apparentPowerSum += apparentPower[i];
                }
            }
            else
                lambdas.phases[i] = qQNaN();
        }
        if (activePower[sumIndex] == 0)
            //This is necessary if none of the phase is active, then activePower.sum and apparentPowerSum is also 0.
            //Todo: Then lambda of sum is 0?
            lambdas.sum = 0.0;
        else if (apparentPowerSum == 0)
            lambdas.sum = qQNaN();
        else
            lambdas.sum = limitValueToPlusMinusOne(activePower[sumIndex] / apparentPowerSum);
    }
    return lambdas;
}

double LambdaCalculator::limitValueToPlusMinusOne(const double &value)
{
    if(fabs(value) > 1)
        return std::signbit(value) ? -1.0 : 1.0;
    else
        return value;
}

PhaseSumValues LambdaCalculator::lambdaFor3LW(double activePowerSum, double apparentPowerSum)
{
    PhaseSumValues lambdas;
    for(double &phaseLambda : lambdas.phases)
        phaseLambda = qQNaN();
    if (apparentPowerSum == 0)
        lambdas.sum = qQNaN();
    else
        lambdas.sum = limitValueToPlusMinusOne(activePowerSum / apparentPowerSum);

    return lambdas;
}
