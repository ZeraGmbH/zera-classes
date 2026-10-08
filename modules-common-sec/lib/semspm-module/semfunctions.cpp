#include "semfunctions.h"
#include "unithelper.h"

const QHash<QString, double> &SemFunctions::getUnitFactorHash() const
{
    return cUnitHelper::getEnergyUnitFactorHash();
}

const QString SemFunctions::getMeasuredValueLabel() const
{
    return "Energy";
}

const QString SemFunctions::getUnitSelectDescription() const
{
    return QString("Energy unit:\n"
                   "Valid values depend on power type selected:\n"
                   "* Active power (P): 'MWh', 'kWh', 'Wh'\n"
                   "* Reactive power (Q): 'MVarh', 'kVarh', 'Varh'\n"
                   "* Apparent power (S): 'MVAh', 'kVAh', 'VAh'");
}

const QString &SemFunctions::getMainUnit(const QString &energyUnit, const QString &powerUnit) const
{
    Q_UNUSED(powerUnit)
    return energyUnit;
}

const QStringList &SemFunctions::getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator)
{
    Q_UNUSED(powerValidator)
    return energyValidator;
}

double SemFunctions::adjustReferenceValue(const double &refValue, const double &time)
{
    Q_UNUSED(time);
    return refValue;
}
