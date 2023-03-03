#ifndef MEASMODEPHASESETSTRATEGY2WIRE_H
#define MEASMODEPHASESETSTRATEGY2WIRE_H

#include "measmodephasesetstrategyphasesvar.h"

class MeasModePhaseSetStrategy2Wire : public MeasModePhaseSetStrategyPhasesVar
{
public:
    MeasModePhaseSetStrategy2Wire(MModePhaseMask phaseMask) : MeasModePhaseSetStrategyPhasesVar(phaseMask, 1) {}
    bool tryChangeMask(MModePhaseMask phaseMask) override
    {
        if(phaseMask.count() == 0)
            return false;
        return MeasModePhaseSetStrategyPhasesVar::tryChangeMask(phaseMask);
    }

};

#endif // MEASMODEPHASESETSTRATEGY2WIRE_H
