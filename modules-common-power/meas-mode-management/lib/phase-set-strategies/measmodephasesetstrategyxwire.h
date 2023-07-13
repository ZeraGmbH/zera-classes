#ifndef MEASMODEPHASESETSTRATEGYXWIRE_H
#define MEASMODEPHASESETSTRATEGYXWIRE_H

#include "measmodephasesetstrategyphasesvar.h"

class MeasModePhaseSetStrategyXWire : public MeasModePhaseSetStrategyPhasesVar
{
public:
    MeasModePhaseSetStrategyXWire(MModePhaseMask phaseMask, int activeMeasSysCount);
    int getActiveMeasSysCount() const override;
};

#endif // MEASMODEPHASESETSTRATEGYXWIRE_H
