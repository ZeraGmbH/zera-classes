#include "measmodephasesetstrategyphasesvar.h"

MeasModePhaseSetStrategyPhasesVar::MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask) :
    m_phaseMask(phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesVar::tryChangeMask(MModePhaseMask phaseMask)
{
    m_phaseMask = phaseMask;
    return true;
}

bool MeasModePhaseSetStrategyPhasesVar::tryChangePhase(int phase)
{
    if(phase<0 || phase>=MeasPhaseCount)
        return false;
    m_phaseMask = (1<<(MeasPhaseCount-phase-1));
    return true;
}

MModePhaseMask MeasModePhaseSetStrategyPhasesVar::getCurrPhaseMask()
{
    return m_phaseMask;
}
