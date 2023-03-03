#ifndef MEASMODEPHASESETSTRATEGY_H
#define MEASMODEPHASESETSTRATEGY_H

#include "measmodecatalog.h"
#include <bitset>

typedef std::bitset<MeasPhaseCount> MModePhaseMask;

class MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategy(int activeMeasSysCount) : m_activeMeasSysCount(activeMeasSysCount) {};
    virtual ~MeasModePhaseSetStrategy() = default;
    virtual bool canChangePhaseMask() = 0;
    virtual bool tryChangeMask(MModePhaseMask phaseMask) = 0;
    virtual MModePhaseMask getCurrPhaseMask() = 0;
    virtual int getActiveMeasSysCount() { return m_activeMeasSysCount; }
protected:
    int m_activeMeasSysCount;
};

#endif // MEASMODEPHASESETSTRATEGY_H
