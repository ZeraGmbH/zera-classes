#ifndef CUNITHELPER_H
#define CUNITHELPER_H

#include <QString>
#include <QHash>

class cUnitHelper
{
public:
    static QString getNewPowerUnit(const QString &powerType, const QString &currentPowerUnit);
    /**
     * @brief getEnergyUnit Helper on change of powerType
     * @param powerType string containing 'P' / 'Q' / 'S'
     * @param currentPowerUnit If set, getPowerUnit tries to keep 10³ prefix in new unit
     * @param powerToEnergyTimeSeconds hint for postfix 1 -> 's' / 3600 -> 'h'
     * @return
     */
    static QString getNewEnergyUnit(const QString &powerType, const QString &currentPowerUnit, int powerToEnergyTimeSeconds=3600);

    static const QHash<QString /*unit e.g kW*/, double /*factor e.g 1000*/> &getPowerUnitFactorHash();
    static const QHash<QString /*unit e.g kWh*/, double /*factor e.g 1000*/> &getEnergyUnitFactorHash();

    static const QStringList &getPowerUnits(const QString &powerType);

private:
    static const QStringList m_activePowerUnits;
    static const QStringList m_reactivePowerUnits;
    static const QStringList m_apparentPowerUnits;
    static const QHash<QString, double> m_powerUnitFactors;
    static const QHash<QString, double> m_energyUnitFactors;
};

#endif // CUNITHELPER_H
