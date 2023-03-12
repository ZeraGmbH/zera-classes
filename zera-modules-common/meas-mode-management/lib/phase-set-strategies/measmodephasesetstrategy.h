#ifndef MEASMODEPHASESETSTRATEGY_H
#define MEASMODEPHASESETSTRATEGY_H

#include "measmodecatalog.h"
#include <bitset>
#include <memory>

typedef std::bitset<MeasPhaseCount> MModePhaseMask;

class MeasModePhaseSetStrategy
{
public:
    MeasModePhaseSetStrategy(int activeMeasSysCount, MModePhaseMask phaseMask) :
        m_measSysCount(activeMeasSysCount),
        m_phaseMask(phaseMask)
        {};
    virtual ~MeasModePhaseSetStrategy() = default;

    virtual bool isVarMask() = 0;
    virtual bool canChangeMask(MModePhaseMask phaseMask) = 0;
    virtual bool tryChangeMask(MModePhaseMask phaseMask) = 0;
    virtual int getActiveMeasSysCount() const { return m_measSysCount; }
    virtual int getMaxMeasSysCount() const { return m_measSysCount; }

    MModePhaseMask getCurrPhaseMask() const { return m_phaseMask; }
protected:
    int m_measSysCount;
    MModePhaseMask m_phaseMask;
};

typedef std::unique_ptr<MeasModePhaseSetStrategy> MeasModePhaseSetStrategyPtr;

#endif // MEASMODEPHASESETSTRATEGY_H
