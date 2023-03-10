#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"
#include <QObject>

class MeasMode : public QObject
{
    Q_OBJECT
public:
    MeasMode() = default;
    MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter);
    QString getName() const;
    int getDspSelectCode() const;
    bool canChangeMask(QString mask);
    bool tryChangeMask(QString mask);
    QString getCurrentMask() const;
    bool isPhaseActive(int phase) const;
    int getMeasSysCount() const;
    int getActiveMeasSysCount() const;
    bool isValid() const;

private:
    bool calcBinMask(QString mask, MModePhaseMask &binMask);
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode = 0;
    MeasModePhaseSetStrategyPtr m_measModePhaseSetter;
    int m_measSysCount;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
