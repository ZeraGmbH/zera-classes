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
    LambdaCalcDelegate(bool activePowerMeasModeAvail, QList<VfModuleActvalue*> actvalue);
    PhaseSumValues getLambdaValues();
public slots:
    void onActivePower1Change(QVariant power);
    void onActivePower2Change(QVariant power);
    void onActivePower3Change(QVariant power);
    void onActivePowerSumChange(QVariant power);
    void onApparentPower1Change(QVariant power);
    void onApparentPower2Change(QVariant power);
    void onApparentPower3Change(QVariant power);
    void onApparentPowerSumChange(QVariant power);
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
    QList<VfModuleActvalue*> m_actValues;
};

#endif // LAMBDACALCDELEGATE_H
