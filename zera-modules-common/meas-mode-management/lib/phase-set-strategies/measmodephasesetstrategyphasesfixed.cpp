#include "measmodephasesetstrategyphasesfixed.h"

MeasModePhaseSetStrategyPhasesFixed::MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategy(activeMeasSysCount),
    m_phaseMask(phaseMask)
{
}

bool MeasModePhaseSetStrategyPhasesFixed::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask);
    return false;
}

MModePhaseMask MeasModePhaseSetStrategyPhasesFixed::getCurrPhaseMask()
{
    return m_phaseMask;
}
