#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"
#include <QObject>
#include <memory>

class MeasMode : public QObject
{
    Q_OBJECT
public:
    MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter);
    cMeasModeInfo getInfo() const;
    int getDspSelectCode();
    bool tryChangeMask(QString mask);
    QString getCurrentMask();
    bool isPhaseActive(int phase);
    int getActiveMeasSysCount();
    bool isValid();
private:
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode;
    MeasModePhaseSetStrategyPtr m_measModePhaseSetter;
    int m_measSysCount;
    bool m_bValid;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
