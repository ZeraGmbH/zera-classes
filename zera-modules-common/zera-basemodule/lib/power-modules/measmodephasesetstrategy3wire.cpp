#include "measmodephasesetstrategy3wire.h"

// Currently 3-wire modes do not support phase move
bool MeasModePhaseSetStrategy3Wire::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask);
    return false;
}

bool MeasModePhaseSetStrategy3Wire::tryChangePhase(int phase)
{
    Q_UNUSED(phase)
    return false;
}

MModePhaseMask MeasModePhaseSetStrategy3Wire::getCurrPhaseMask()
{
    return MModePhaseMask("101");
}
