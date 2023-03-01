#ifndef MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H
#define MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H

#include "measmodephasesetstrategyphasesfixed.h"

// This is just temporay and can go once we support variable phases on 2-wire
class MeasModePhaseSetStrategy2WireFixedPhase : public MeasModePhaseSetStrategyPhasesFixed
{
public:
    MeasModePhaseSetStrategy2WireFixedPhase(int phase);
protected:
    MModePhaseMask maskFromPhase(int phase);
};

#endif // MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H
