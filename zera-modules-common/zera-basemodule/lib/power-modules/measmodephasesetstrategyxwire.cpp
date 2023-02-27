#include "measmodephasesetstrategyxwire.h"

MeasModePhaseSetStrategyXWire::MeasModePhaseSetStrategyXWire(MModePhaseMask phaseMask) :
    m_phaseMask(phaseMask)
{
}

bool MeasModePhaseSetStrategyXWire::tryChangeMask(MModePhaseMask phaseMask)
{
    m_phaseMask = phaseMask;
    return true;
}

bool MeasModePhaseSetStrategyXWire::tryChangePhase(int phase)
{
    if(phase<0 || phase>=MeasPhaseCount)
        return false;
    m_phaseMask = (1<<(MeasPhaseCount-phase-1));
    return true;
}

MModePhaseMask MeasModePhaseSetStrategyXWire::getCurrPhaseMask()
{
    return m_phaseMask;
}
