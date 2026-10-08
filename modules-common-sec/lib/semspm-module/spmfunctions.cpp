#include "spmfunctions.h"
#include "unithelper.h"

const QHash<QString, double> &SpmFunctions::getUnitFactorHash() const
{
    return cUnitHelper::getPowerUnitFactorHash();
}

const QString SpmFunctions::getMeasuredValueLabel() const
{
    return "Power";
}

const QString SpmFunctions::getUnitSelectDescription() const
{
    return QString("Power unit:\n"
                   "Valid values depend on power type selected:\n"
                   "* Active power (P): 'MW', 'kW', 'W'\n"
                   "* Reactive power (Q): 'MVar', 'kVar', 'Var'\n"
                   "* Apparent power (S): 'MVA', 'kVA', 'VA'");
}

const QString &SpmFunctions::getMainUnit(const QString &energyUnit, const QString &powerUnit) const
{
    Q_UNUSED(energyUnit)
    return powerUnit;
}

const QStringList &SpmFunctions::getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator)
{
    Q_UNUSED(energyValidator)
    return powerValidator;
}

double SpmFunctions::adjustReferenceValue(const double &refValue, const double &time)
{
    return refValue * 3600.0 / time;
}
