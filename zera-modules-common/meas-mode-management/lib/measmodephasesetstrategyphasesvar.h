#ifndef MEASMODEPHASESETSTRATEGYPHASESVAR_H
#define MEASMODEPHASESETSTRATEGYPHASESVAR_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyPhasesVar : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyPhasesVar(MModePhaseMask phaseMask);
    bool canChangePhaseMask() override { return true; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGYPHASESVAR_H
