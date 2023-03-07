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
    bool tryChangeMask(QString mask);
    QString getCurrentMask() const;
    bool isPhaseActive(int phase) const;
    int getActiveMeasSysCount() const;
    bool isValid() const;
private:
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode;
    MeasModePhaseSetStrategyPtr m_measModePhaseSetter;
    int m_measSysCount;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
