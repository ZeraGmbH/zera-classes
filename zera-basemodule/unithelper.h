#ifndef CUNITHELPER_H
#define CUNITHELPER_H

#include <QString>

/**
 * @brief The cUnitHelper class is a tiny static helper class for unit calculations
 */
class cUnitHelper
{
public:
    /**
     * @brief getNewPowerUnit Helper on change of powerType
     * @param powerType string containing 'P' / 'Q' / 'S'
     * @param currentPowerUnit If set, getPowerUnit tries to keep 10³ prefix in new unit
     * @return unit string
     */
    static QString getNewPowerUnit(const QString &powerType, const QString &currentPowerUnit);
    /**
     * @brief getEnergyUnit Helper on change of powerType
     * @param powerType string containing 'P' / 'Q' / 'S'
     * @param currentPowerUnit If set, getPowerUnit tries to keep 10³ prefix in new unit
     * @param powerToEnergyTimeSeconds hint for postfix 1 -> 's' / 3600 -> 'h'
     * @return
     */
    static QString getNewEnergyUnit(const QString &powerType, const QString &currentPowerUnit, int powerToEnergyTimeSeconds=3600);
};

#endif // CUNITHELPER_H
