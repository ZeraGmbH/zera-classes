#ifndef MEASMODEPHASESETSTRATEGY2WIRE_H
#define MEASMODEPHASESETSTRATEGY2WIRE_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategy2Wire : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategy2Wire(MModePhaseMask phaseMask);
    bool canChangePhaseMask() override { return true; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    bool tryChangeMaskPriv(MModePhaseMask phaseMask);
    bool tryChangePhaseTwoSetForOverlappedChange(MModePhaseMask phaseMask);
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGY2WIRE_H
