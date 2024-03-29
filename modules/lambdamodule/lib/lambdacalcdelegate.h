#ifndef LAMBDACALCDELEGATE_H
#define LAMBDACALCDELEGATE_H

#include "lambdacalculator.h"
#include "vfmoduleactvalue.h"
#include "measmodeinfo.h"
#include <QVariant>
#include <QObject>

class LambdaCalcDelegate : public QObject
{
    Q_OBJECT
public:
    LambdaCalcDelegate(bool activePowerMeasModeAvail,
                       QList<VfModuleActvalue*> veinLambdaActValues,
                       QList<VfModuleActvalue*> veinLoadTypeValues);
    PhaseSumValues getLambdaValues();
    void handleActivePowerChange(int phaseOrSum, QVariant power);
    void handleReactivePowerChange(int phaseOrSum, QVariant power);
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

    QVector<double> m_activePowerValues = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);
    QVector<double> m_reactivePowerValues = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);
    QVector<double> m_apparentPowerValues = QVector<double>(MeasPhaseCount+SumValueCount, 0.0);

    QString m_activePowerMeasMode = "4LW";
    QString m_activePowerPhaseMask = "111";
    QList<VfModuleActvalue*> m_veinLambdaActValues;
    QList<VfModuleActvalue*> m_veinLoadTypeValues;
};

#endif // LAMBDACALCDELEGATE_H
