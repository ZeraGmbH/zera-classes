#ifndef MEASMODEPHASESETSTRATEGYPHASESFIXED_H
#define MEASMODEPHASESETSTRATEGYPHASESFIXED_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesFixed : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask, int activeMeasSysCount);
    bool isVarMask() override { return false; }
    bool canChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangeMask(MModePhaseMask phaseMask) override;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESFIXED_H
