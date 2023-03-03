#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"
#include <QObject>
#include <memory>

class MeasMode : public QObject
{
    Q_OBJECT
public:
    MeasMode(QString modeName, int dspSelectCode, int measSysCount, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter);
    cMeasModeInfo getInfo() const;
    int getDspSelectCode();
    void tryChangeMask(QString mask);
    QString getCurrentMask();
    bool isValid();
signals:
    void sigMaskChanged();
    void sigMaskChangeFailed();
private:
    cMeasModeInfo m_measModeInfo;
    int m_dspSelectCode;
    std::unique_ptr<MeasModePhaseSetStrategy> m_measModePhaseSetter;
    int m_measSysCount;
    bool m_bValid;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
