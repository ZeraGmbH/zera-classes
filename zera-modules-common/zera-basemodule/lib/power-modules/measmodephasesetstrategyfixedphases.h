#ifndef MEASMODEPHASESETSTRATEGYFIXEDPHASES_H
#define MEASMODEPHASESETSTRATEGYFIXEDPHASES_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyFixedPhases : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyFixedPhases(MModePhaseMask phaseMask);
    bool canChangePhaseMask() override { return false; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGYFIXEDPHASES_H
