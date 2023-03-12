#include "measmodephasesetstrategyphasesvar.h"

MeasModePhaseSetStrategyPhasesVar::MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount, phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesVar::canChangeMask(MModePhaseMask phaseMask)
{
    if(int(phaseMask.count()) > getMaxMeasSysCount())
        return false;
    return true;
}

bool MeasModePhaseSetStrategyPhasesVar::tryChangeMask(MModePhaseMask phaseMask)
{
    if(!canChangeMask(phaseMask))
        return false;
    m_phaseMask = phaseMask;
    return true;
}
