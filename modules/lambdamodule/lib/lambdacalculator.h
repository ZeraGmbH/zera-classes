#ifndef LAMBDACALCULATOR_H
#define LAMBDACALCULATOR_H

#include <QVector>

struct PhaseSumValues
{
    PhaseSumValues();
    QVector<double> phases;
    double sum = 0.0;
    QVector<QString> phaseLoadTypes;
    QString sumLoadType;
};

class LambdaCalculator
{
public:
    static PhaseSumValues calculateLambdaValues(const QVector<double> &activePower,
                                                const QVector<double> &reactivePower,
                                                const QVector<double> &apparentPower,
                                                const QString &measModeActivePower,
                                                const QString &phaseMaskActivePower);
    static QString IndText;
    static QString CapText;
    static constexpr double MaxPowerFactorForTypeText = 0.996; // ~5°
private:
    static double limitValueToPlusMinusOne(const double &value);
    static PhaseSumValues lambdaFor3LW(double activePowerSum, double apparentPowerSum);
};

#endif // LAMBDACALCULATOR_H
