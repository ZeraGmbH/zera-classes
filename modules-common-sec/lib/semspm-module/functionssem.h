#ifndef FUNCTIONSSEM_H
#define FUNCTIONSSEM_H

#include "abstractsemspmfunctions.h"

class FunctionsSem : public AbstractSemSpmFunctions
{
public:
    const QHash<QString, double> &getUnitFactorHash() const override;

    const QString getMeasuredValueLabel() const override;
    const QString getUnitSelectDescription() const override;

    const QString &getMainUnit(const QString &energyUnit, const QString &powerUnit) const override;
    const QStringList &getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator) override;

    double adjustReferenceValue(const double &refValue, const double &measDurationSeconds) override;
};

#endif // FUNCTIONSSEM_H
