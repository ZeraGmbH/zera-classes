#ifndef CUNITHELPER_H
#define CUNITHELPER_H

#include <QString>
#include <QHash>

class cUnitHelper
{
public:
    static QString deduceMatchingPowerUnit(const QString &powerType, const QString &currentPowerUnit);

    static const QHash<QString /*unit e.g kW*/, double /*factor e.g 1000*/> &getPowerUnitFactorHash();
    static const QHash<QString /*unit e.g kWh*/, double /*factor e.g 1000*/> &getEnergyUnitFactorHash();

    static const QStringList &getPowerUnits(const QString &powerType);
    static const QStringList &getDUTConstantUnits(const QString &powerType);

private:
    static const QStringList m_activeDutConstantUnits;
    static const QStringList m_reactiveDutConstantUnits;
    static const QStringList m_apparentDutConstantUnits;

    static const QStringList m_activePowerUnits;
    static const QStringList m_reactivePowerUnits;
    static const QStringList m_apparentPowerUnits;

    static const QHash<QString, double> m_powerUnitFactors;
    static const QHash<QString, double> m_energyUnitFactors;
};

#endif // CUNITHELPER_H
