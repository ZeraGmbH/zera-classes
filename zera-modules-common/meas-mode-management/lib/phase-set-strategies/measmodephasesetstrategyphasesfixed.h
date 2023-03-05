#ifndef MEASMODEPHASESETSTRATEGYPHASESFIXED_H
#define MEASMODEPHASESETSTRATEGYPHASESFIXED_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesFixed : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask, int activeMeasSysCount);
    bool canChangePhaseMask() override { return false; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESFIXED_H
