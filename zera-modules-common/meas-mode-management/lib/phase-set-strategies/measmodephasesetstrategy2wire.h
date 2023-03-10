#ifndef MEASMODEPHASESETSTRATEGY2WIRE_H
#define MEASMODEPHASESETSTRATEGY2WIRE_H

#include "measmodephasesetstrategyphasesvar.h"

class MeasModePhaseSetStrategy2Wire : public MeasModePhaseSetStrategyPhasesVar
{
public:
    MeasModePhaseSetStrategy2Wire() : MeasModePhaseSetStrategyPhasesVar(MModePhaseMask("001"), 1) {}
};

#endif // MEASMODEPHASESETSTRATEGY2WIRE_H
