#include "measmodephasesetstrategyfixedphases.h"

MeasModePhaseSetStrategyFixedPhases::MeasModePhaseSetStrategyFixedPhases(MModePhaseMask phaseMask) :
    m_phaseMask(phaseMask)
{
}

bool MeasModePhaseSetStrategyFixedPhases::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask);
    return false;
}

bool MeasModePhaseSetStrategyFixedPhases::tryChangePhase(int phase)
{
    Q_UNUSED(phase)
    return false;
}

MModePhaseMask MeasModePhaseSetStrategyFixedPhases::getCurrPhaseMask()
{
    return m_phaseMask;
}
