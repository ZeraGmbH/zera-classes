#include "measmodephasesetstrategyxwire.h"

MeasModePhaseSetStrategyXWire::MeasModePhaseSetStrategyXWire(MModePhaseMask phaseMask, int activeMeasSysCount) :
    MeasModePhaseSetStrategyPhasesVar(phaseMask, activeMeasSysCount)
{
}

int MeasModePhaseSetStrategyXWire::getActiveMeasSysCount() const
{
    return m_phaseMask.count();
}
