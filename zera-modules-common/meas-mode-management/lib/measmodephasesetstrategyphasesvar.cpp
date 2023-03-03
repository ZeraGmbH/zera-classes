#include "measmodephasesetstrategyphasesvar.h"

MeasModePhaseSetStrategyPhasesVar::MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount),
    m_phaseMask(phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesVar::tryChangeMask(MModePhaseMask phaseMask)
{
    if(int(phaseMask.count()) > m_activeMeasSysCount)
        return false;
    m_phaseMask = phaseMask;
    return true;
}

MModePhaseMask MeasModePhaseSetStrategyPhasesVar::getCurrPhaseMask()
{
    return m_phaseMask;
}

