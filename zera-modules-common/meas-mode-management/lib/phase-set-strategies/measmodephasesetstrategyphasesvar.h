#ifndef MEASMODEPHASESETSTRATEGYPHASESVAR_H
#define MEASMODEPHASESETSTRATEGYPHASESVAR_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesVar : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask, int activeMeasSysCount);
    bool canChangePhaseMask() override { return true; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESVAR_H
