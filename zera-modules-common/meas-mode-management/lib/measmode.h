#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"

class MeasMode
{
public:
    MeasMode() = default;
    MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter);
    QString getName() const;
    int getDspSelectCode() const;
    bool hasVarMask() const;
    bool canChangeMask(QString mask) const;
    bool tryChangeMask(QString mask);
    QString getCurrentMask() const;
    bool isPhaseActive(int phase) const;
    int getMeasSysCount() const;
    int getActiveMeasSysCount() const;
    int getMaxMeasSysCount() const;
    bool isValid() const;

private:
    bool calcBinMask(QString mask, MModePhaseMask &binMask) const;
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode = 0;
    MeasModePhaseSetStrategyPtr m_measModePhaseSetter;
    int m_measSysCount;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
