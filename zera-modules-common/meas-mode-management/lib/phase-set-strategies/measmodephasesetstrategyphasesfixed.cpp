#include "measmodephasesetstrategyphasesfixed.h"

MeasModePhaseSetStrategyPhasesFixed::MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount, phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesFixed::canChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask);
    return false;
}

bool MeasModePhaseSetStrategyPhasesFixed::tryChangeMask(MModePhaseMask phaseMask)
{
    return canChangeMask(phaseMask);
}
