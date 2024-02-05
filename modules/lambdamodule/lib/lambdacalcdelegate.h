#ifndef LAMBDACALCDELEGATE_H
#define LAMBDACALCDELEGATE_H

#include "lambdacalculator.h"
#include "vfmoduleactvalue.h"
#include <QVariant>
#include <QObject>

class LambdaCalcDelegate : public QObject
{
    Q_OBJECT
public:
    LambdaCalcDelegate(bool activePowerMeasModeAvail, QList<VfModuleActvalue*> veinLambdaActValues);
    PhaseSumValues getLambdaValues();
    void handleActivePowerChange(int phaseOrSum, QVariant power);
    void handleApparentPowerChange(int phaseOrSum, QVariant power);
public slots:
    void onActivePowerMeasModeChange(QVariant measMode);
    void onActivePowerPhaseMaskChange(QVariant phaseMask);
signals:
    void measuring(int);
private:
    void updateLambdaValues();
    bool m_activePowerMeasModeAvail = false;
    PhaseSumValues m_lambdaValues;
    PhaseSumValues m_activePowerValues;
    PhaseSumValues m_apparentPowerValues;
    QString m_activePowerMeasMode = "4LW";
    QString m_activePowerPhaseMask = "111";
    QList<VfModuleActvalue*> m_veinLambdaActValues;
};

#endif // LAMBDACALCDELEGATE_H
