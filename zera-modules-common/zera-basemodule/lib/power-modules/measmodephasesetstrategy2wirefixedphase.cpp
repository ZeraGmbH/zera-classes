#include "measmodephasesetstrategy2wirefixedphase.h"

MeasModePhaseSetStrategy2WireFixedPhase::MeasModePhaseSetStrategy2WireFixedPhase(int phase) :
    MeasModePhaseSetStrategyFixedPhases(maskFromPhase(phase))
{
}

MModePhaseMask MeasModePhaseSetStrategy2WireFixedPhase::maskFromPhase(int phase)
{
    if(phase<0 || phase>=MeasPhaseCount)
        return 0;
    return (1<<(MeasPhaseCount-phase-1));
}
