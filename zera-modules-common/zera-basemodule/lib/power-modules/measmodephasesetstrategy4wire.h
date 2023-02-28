#ifndef MEASMODEPHASESETSTRATEGY4WIRE_H
#define MEASMODEPHASESETSTRATEGY4WIRE_H

#include "measmodephasesetstrategyfixedphases.h"

class MeasModePhaseSetStrategy4Wire : public MeasModePhaseSetStrategyFixedPhases
{
public:
    MeasModePhaseSetStrategy4Wire() : MeasModePhaseSetStrategyFixedPhases(MModePhaseMask("111")) {}
};

#endif // MEASMODEPHASESETSTRATEGY4WIRE_H
