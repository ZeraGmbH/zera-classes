#ifndef MEASMODEPHASESETSTRATEGYPHASESVAR_H
#define MEASMODEPHASESETSTRATEGYPHASESVAR_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesVar : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask, int activeMeasSysCount);
    bool isVarMask() override { return true; }
    bool canChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangeMask(MModePhaseMask phaseMask) override;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESVAR_H
