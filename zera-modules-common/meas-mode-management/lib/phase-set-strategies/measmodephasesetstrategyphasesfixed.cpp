#include "measmodephasesetstrategyphasesfixed.h"

MeasModePhaseSetStrategyPhasesFixed::MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount, phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesFixed::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask);
    return false;
}
