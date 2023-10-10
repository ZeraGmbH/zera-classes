#include "lambdacalculator.h"
#include "measmodecatalog.h"

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
                lambdas.phases[i] = activePower.phases[i] / apparentPower.phases[i];
    }
    if (apparentPower.sum == 0)
        lambdas.sum = qSNaN();
    else
        lambdas.sum = activePower.sum / apparentPower.sum;
    return lambdas;
}
