#ifndef SEMFUNCTIONS_H
#define SEMFUNCTIONS_H

#include "abstractsemspmfunctions.h"

class SemFunctions : public AbstractSemSpmFunctions
{
public:
    const QHash<QString, double> &getUnitFactorHash() const override;

    const QString getMeasuredValueLabel() const override;
    const QString getUnitSelectDescription() const override;

    const QString &getMainUnit(const QString &energyUnit, const QString &powerUnit) const override;
    const QStringList &getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator) override;

    double adjustReferenceValue(const double &refValue, const double &time) override;
};

#endif // SEMFUNCTIONS_H
