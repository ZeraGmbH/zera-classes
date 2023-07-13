#include "measmodephasesetstrategy2wirefixedphase.h"

MeasModePhaseSetStrategy2WireFixedPhase::MeasModePhaseSetStrategy2WireFixedPhase(int phase, int measSystemCount) :
    MeasModePhaseSetStrategyPhasesFixed(maskFromPhase(phase, measSystemCount), 1),
    m_measSystemCount(measSystemCount)
{
}

MModePhaseMask MeasModePhaseSetStrategy2WireFixedPhase::maskFromPhase(int phase, int measSystemCount)
{
    if(phase<0 || phase>=measSystemCount)
        return 0;
    return (1<<(measSystemCount-phase-1));
}
