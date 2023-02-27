#include "measmodephasesetstrategy2wire.h"

MeasModePhaseSetStrategy2Wire::MeasModePhaseSetStrategy2Wire(MModePhaseMask phaseMask)
{
    tryChangeMaskPriv(phaseMask);
}

bool MeasModePhaseSetStrategy2Wire::tryChangeMask(MModePhaseMask phaseMask)
{
    return tryChangeMaskPriv(phaseMask);
}

bool MeasModePhaseSetStrategy2Wire::tryChangePhase(int phase)
{
    if(phase<0 || phase>=MeasPhaseCount)
        return false;
    m_phaseMask = (1<<(MeasPhaseCount-phase-1));
    return true;
}

MModePhaseMask MeasModePhaseSetStrategy2Wire::getCurrPhaseMask()
{
    return m_phaseMask;
}

bool MeasModePhaseSetStrategy2Wire::tryChangeMaskPriv(MModePhaseMask phaseMask)
{
    if(phaseMask.count() == 0 || phaseMask.count() > 2)
        return false;
    if(phaseMask.count() == 2)
        return tryChangePhaseTwoSetForOverlappedChange(phaseMask);
    m_phaseMask = phaseMask;
    return true;
}

bool MeasModePhaseSetStrategy2Wire::tryChangePhaseTwoSetForOverlappedChange(MModePhaseMask phaseMask)
{
    MModePhaseMask exorMask = m_phaseMask ^ phaseMask;
    if(exorMask.count() != 1)
        return false;
    m_phaseMask = exorMask;
    return true;
}

