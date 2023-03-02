#ifndef MEASMODEPHASESETSTRATEGYPHASESFIXED_H
#define MEASMODEPHASESETSTRATEGYPHASESFIXED_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesFixed : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesFixed(MModePhaseMask phaseMask);
    bool canChangePhaseMask() override { return false; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESFIXED_H
