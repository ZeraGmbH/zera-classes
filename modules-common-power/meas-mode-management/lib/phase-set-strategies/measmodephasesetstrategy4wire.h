#ifndef MEASMODEPHASESETSTRATEGY4WIRE_H
#define MEASMODEPHASESETSTRATEGY4WIRE_H

#include "measmodephasesetstrategyphasesfixed.h"

class MeasModePhaseSetStrategy4Wire : public MeasModePhaseSetStrategyPhasesFixed
{
public:
    MeasModePhaseSetStrategy4Wire() : MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask("111"), 3) {}
};

#endif // MEASMODEPHASESETSTRATEGY4WIRE_H
