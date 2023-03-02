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

MModePhaseMask MeasModePhaseSetStrategyPhasesVar::getCurrPhaseMask()
{
    return m_phaseMask;
}
