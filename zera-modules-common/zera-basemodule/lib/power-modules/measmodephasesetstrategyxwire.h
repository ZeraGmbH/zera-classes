#ifndef MEASMODEPHASESETSTRATEGYXWIRE_H
#define MEASMODEPHASESETSTRATEGYXWIRE_H

#include "measmodephasesetstrategy.h"

class MeasModePhaseSetStrategyXWire : public MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategyXWire(MModePhaseMask phaseMask);
    bool canChangePhaseMask() override { return true; }
    bool tryChangeMask(MModePhaseMask phaseMask) override;
    bool tryChangePhase(int phase)  override;
    MModePhaseMask getCurrPhaseMask()  override;
private:
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGYXWIRE_H
