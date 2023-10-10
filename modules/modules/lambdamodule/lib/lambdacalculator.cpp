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

    for(int i = 0; i < MeasPhaseCount; i++) {
        if (info.isThreeWire())
            lambdas.phases[i] = qSNaN();
        else if (phaseMask.size() > i && phaseMask.at(i) == "1")
            if (apparentPower.phases[i] == 0)
                lambdas.phases[i] = qSNaN();
            else
                lambdas.phases[i] = limitValueToPlusMinusOne(activePower.phases[i] / apparentPower.phases[i]);
    }
    if (apparentPower.sum == 0)
        lambdas.sum = qSNaN();
    else
        lambdas.sum = limitValueToPlusMinusOne(activePower.sum / apparentPower.sum);

    return lambdas;
}

double LambdaCalculator::limitValueToPlusMinusOne(const double &value)
{
    if(fabs(value) > 1)
        return std::signbit(value) ? -1.0 : 1.0;
    else
        return value;
}
