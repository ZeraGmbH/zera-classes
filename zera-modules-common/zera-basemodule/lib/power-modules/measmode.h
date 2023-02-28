#ifndef MEASMODE_H
#define MEASMODE_H

#include "measmodephasesetstrategy.h"
#include <QObject>
#include <memory>

class MeasMode : public QObject
{
    Q_OBJECT
public:
    MeasMode(QString modeName, measmodes dspMode, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter);
    const cMeasModeInfo *getInfo();
    measmodes getDspMode();
    void tryChangeMask(MModePhaseMask phaseMask);
    MModePhaseMask getCurrentMask();
signals:
    void sigMaskTransactionFinished();
private:
    const cMeasModeInfo *m_measModeStaticInfo = nullptr;
    measmodes m_dspMode;
    std::unique_ptr<MeasModePhaseSetStrategy> m_measModePhaseSetter;
};

typedef std::shared_ptr<MeasMode> MeasModePtr;

#endif // MEASMODE_H
