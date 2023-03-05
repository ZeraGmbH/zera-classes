#include "measmodephasesetstrategyphasesvar.h"

MeasModePhaseSetStrategyPhasesVar::MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount, phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesVar::tryChangeMask(MModePhaseMask phaseMask)
{
    if(int(phaseMask.count()) > m_activeMeasSysCount)
        return false;
    m_phaseMask = phaseMask;
    return true;
}
