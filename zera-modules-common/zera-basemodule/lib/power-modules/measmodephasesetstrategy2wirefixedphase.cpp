#include "measmodephasesetstrategy2wirefixedphase.h"

MeasModePhaseSetStrategy2WireFixedPhase::MeasModePhaseSetStrategy2WireFixedPhase(int phase)
{
    m_phaseMask = MModePhaseMask("100");
    if(phase<0 || phase>=MeasPhaseCount)
        return;
    m_phaseMask = (1<<(MeasPhaseCount-phase-1));
}

bool MeasModePhaseSetStrategy2WireFixedPhase::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask)
    return false;
}

bool MeasModePhaseSetStrategy2WireFixedPhase::tryChangePhase(int phase)
{
    Q_UNUSED(phase)
    return false;
}

MModePhaseMask MeasModePhaseSetStrategy2WireFixedPhase::getCurrPhaseMask()
{
    return m_phaseMask;
}

