#include "lambdacalculator.h"
#include "measmodeinfo.h"

PhaseSumValues::PhaseSumValues() :
    phases(MeasPhaseCount, 0.0)
{
}

PhaseSumValues LambdaCalculator::calculateAllLambdas(const PhaseSumValues &activePower, const PhaseSumValues &apparentPower, QString measModeActivePower, QString phaseMask)
{
    PhaseSumValues lambdas;
    for(int i = 0; i < MeasPhaseCount; i++) {
        if (phaseMask.size() > i && phaseMask.at(i) == "1")
            lambdas.phases[i] = activePower.phases[i] / apparentPower.phases[i];
    }
    lambdas.sum = activePower.sum / apparentPower.sum;
    return lambdas;
}
