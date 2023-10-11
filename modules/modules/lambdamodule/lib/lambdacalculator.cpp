#include "lambdacalculator.h"
#include "measmodecatalog.h"
#include <math.h>

PhaseSumValues::PhaseSumValues() :
    phases(MeasPhaseCount, 0.0)
{
}

PhaseSumValues LambdaCalculator::calculateAllLambdas(const PhaseSumValues &activePower, const PhaseSumValues &apparentPower, QString measModeActivePower, QString phaseMask)
{
    PhaseSumValues lambdas;
    cMeasModeInfo info = MeasModeCatalog::getInfo(measModeActivePower);
    double apparentPowerSum = 0.0;

    if (info.isThreeWire())
        lambdas = lambdaFor3LW(activePower.sum, apparentPower.sum);
    else {
        for(int i = 0; i < MeasPhaseCount; i++) {
            if (phaseMask.size() > i && phaseMask.at(i) == "1") {
                if (apparentPower.phases[i] == 0)
                    lambdas.phases[i] = qSNaN();
                else {
                    lambdas.phases[i] = limitValueToPlusMinusOne(activePower.phases[i] / apparentPower.phases[i]);
                    apparentPowerSum += apparentPower.phases[i];
                }
            }
        }
        if (activePower.sum == 0)
            //This is necessary if none of the phase is active, then activePower.sum and apparentPowerSum is also 0.
            //Todo: Then lambda of sum is 0?
            lambdas.sum = 0.0;
        else if (apparentPowerSum == 0)
            lambdas.sum = qSNaN();
        else
            lambdas.sum = limitValueToPlusMinusOne(activePower.sum / apparentPowerSum);
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
        phaseLambda = qSNaN();
    if (apparentPowerSum == 0)
        lambdas.sum = qSNaN();
    else
        lambdas.sum = limitValueToPlusMinusOne(activePowerSum / apparentPowerSum);

    return lambdas;
}
