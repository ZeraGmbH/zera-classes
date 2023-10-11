#ifndef LAMBDACALCDELEGATE_H
#define LAMBDACALCDELEGATE_H

#include "lambdacalculator.h"
#include <QObject>

class LambdaCalcDelegate : public QObject
{
    Q_OBJECT
public:
    LambdaCalcDelegate(bool activePowerMeasModeAvail);
    PhaseSumValues getLambdaValues();
public slots:
    void onActivePower1Change(double power);
    void onActivePower2Change(double power);
    void onActivePower3Change(double power);
    void onActivePowerSumChange(double power);
    void onApparentPower1Change(double power);
    void onApparentPower2Change(double power);
    void onApparentPower3Change(double power);
    void onApparentPowerSumChange(double power);
    void onActivePowerMeasModeChange(QString measMode);
    void onActivePowerPhaseMaskChange(QString phaseMask);
private:
    void updateLambdaValues();
    bool m_activePowerMeasModeAvail = false;
    PhaseSumValues m_lambdaValues;
    PhaseSumValues m_activePowerValues;
    PhaseSumValues m_apparentPowerValues;
    QString m_activePowerMeasMode = "";
    QString m_activePowerPhaseMask = "111";
};

#endif // LAMBDACALCDELEGATE_H
