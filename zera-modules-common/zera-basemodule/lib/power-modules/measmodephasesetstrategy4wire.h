#ifndef MEASMODEPHASESETSTRATEGY4WIRE_H
#define MEASMODEPHASESETSTRATEGY4WIRE_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategy4Wire : public MeasModePhaseSetStrategy
{
public:
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
};

#endif // MEASMODEPHASESETSTRATEGY4WIRE_H
