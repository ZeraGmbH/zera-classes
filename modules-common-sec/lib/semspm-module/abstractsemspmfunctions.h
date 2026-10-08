#ifndef ABSTRACTSEMSPMFUNCTIONS_H
#define ABSTRACTSEMSPMFUNCTIONS_H

#include <QString>
#include <QHash>

class AbstractSemSpmFunctions
{
public:
    virtual ~AbstractSemSpmFunctions() = default;

    virtual const QHash<QString, double> &getUnitFactorHash() const = 0;

    virtual const QString getMeasuredValueLabel() const = 0;
    virtual const QString getUnitSelectDescription() const = 0;

    virtual const QString &getMainUnit(const QString &energyUnit, const QString &powerUnit) const = 0;
    virtual const QStringList &getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator) = 0;

    virtual double adjustReferenceValue(const double &refValue, const double &time) = 0;
};

#endif // ABSTRACTSEMSPMFUNCTIONS_H
