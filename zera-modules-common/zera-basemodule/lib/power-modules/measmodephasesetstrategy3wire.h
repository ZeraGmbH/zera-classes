#ifndef MEASMODEPHASESETSTRATEGY3WIRE_H
#define MEASMODEPHASESETSTRATEGY3WIRE_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategy3Wire : public MeasModePhaseSetStrategy
{
public:
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
};

#endif // MEASMODEPHASESETSTRATEGY3WIRE_H
