#ifndef MEASMODEPHASESETSTRATEGY_H
#define MEASMODEPHASESETSTRATEGY_H

#include "measmodecatalog.h"
#include <bitset>

typedef std::bitset<MeasPhaseCount> MModePhaseMask;

class MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategy(int activeMeasSysCount, MModePhaseMask phaseMask) :
        m_activeMeasSysCount(activeMeasSysCount),
        m_phaseMask(phaseMask)
        {};
    virtual ~MeasModePhaseSetStrategy() = default;

    virtual bool canChangePhaseMask() = 0;
    virtual bool tryChangeMask(MModePhaseMask phaseMask) = 0;

    MModePhaseMask getCurrPhaseMask() const { return m_phaseMask; }
    virtual int getActiveMeasSysCount() const { return m_activeMeasSysCount; }
protected:
    int m_activeMeasSysCount;
    MModePhaseMask m_phaseMask;
};

#endif // MEASMODEPHASESETSTRATEGY_H
