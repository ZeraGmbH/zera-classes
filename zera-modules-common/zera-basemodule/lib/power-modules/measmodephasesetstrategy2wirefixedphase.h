#ifndef MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H
#define MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategy2WireFixedPhase : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategy2WireFixedPhase(int phase);
    bool canChangePhaseMask() override { return false; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGY2WIREFIXEDPHASE_H
