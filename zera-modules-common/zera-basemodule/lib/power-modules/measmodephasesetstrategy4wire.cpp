#include "measmodephasesetstrategy4wire.h"

bool MeasModePhaseSetStrategy4Wire::tryChangeMask(MModePhaseMask phaseMask)
{
    Q_UNUSED(phaseMask)
    return false;
}

bool MeasModePhaseSetStrategy4Wire::tryChangePhase(int phase)
{
    Q_UNUSED(phase)
    return false;
}

MModePhaseMask MeasModePhaseSetStrategy4Wire::getCurrPhaseMask()
{
    return MModePhaseMask("111");
}
