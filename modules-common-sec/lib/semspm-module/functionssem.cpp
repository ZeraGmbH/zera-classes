#include "functionssem.h"
#include "unithelper.h"

const QHash<QString, double> &FunctionsSem::getUnitFactorHash() const
{
    return UnitHelper::getEnergyUnitFactorHash();
}

const QString FunctionsSem::getMeasuredValueLabel() const
{
    return "Energy";
}

const QString FunctionsSem::getUnitSelectDescription() const
{
    return QString("Energy unit:\n"
                   "Valid values depend on power type selected:\n"
                   "* Active power (P): 'MWh', 'kWh', 'Wh'\n"
                   "* Reactive power (Q): 'MVarh', 'kVarh', 'Varh'\n"
                   "* Apparent power (S): 'MVAh', 'kVAh', 'VAh'");
}

const QString &FunctionsSem::getMainUnit(const QString &energyUnit, const QString &powerUnit) const
{
    Q_UNUSED(powerUnit)
    return energyUnit;
}

const QStringList &FunctionsSem::getUnitValidator(const QStringList &energyValidator, const QStringList &powerValidator)
{
    Q_UNUSED(powerValidator)
    return energyValidator;
}

double FunctionsSem::adjustReferenceValue(const double &refValue, const double &measDurationSeconds)
{
    Q_UNUSED(measDurationSeconds);
    return refValue;
}
