#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"
#include <QObject>
#include <memory>

class MeasMode : public QObject
{
    Q_OBJECT
public:
    MeasMode(QString modeName, int dspSelectCode, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter);
    cMeasModeInfo getInfo() const;
    int getDspSelectCode();
    void tryChangeMask(MModePhaseMask phaseMask);
    MModePhaseMask getCurrentMask();
signals:
    void sigMaskChanged();
    void sigMaskChangeFailed();
private:
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode;
    std::unique_ptr<MeasModePhaseSetStrategy> m_measModePhaseSetter;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
