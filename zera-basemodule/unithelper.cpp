#include "unithelper.h"

QString cUnitHelper::getNewPowerUnit(const QString &powerType, const QString &currentPowerUnit)
{
    QString newUnit;
    // base unit - we assume powerType set
    if (powerType.contains('P')) {
        newUnit = QString("W");
    }
    else if (powerType.contains('Q')) {
        newUnit = QString("Var");
    }
    else if (powerType.contains('S')) {
        newUnit = QString("VA");
    }
    // 10³ prefix
    if(currentPowerUnit.isEmpty() || currentPowerUnit == "Unknown" || currentPowerUnit.startsWith('k')) { // 'k' is default on startup
        newUnit = QString('k') + newUnit;
    }
    else if(currentPowerUnit.startsWith('M')) {
        newUnit = QString('M') + newUnit;
    }
    return newUnit;
}

QString cUnitHelper::getNewEnergyUnit(const QString &powerType, const QString &currentPowerUnit, int powerToEnergyTimeSeconds)
{
    QString postFix;
    switch(powerToEnergyTimeSeconds) {
    case 1:
        postFix = "s";
        break;
    case 3600:
    default:
        postFix = "h";
        break;
    }
    return cUnitHelper::getNewPowerUnit(powerType, currentPowerUnit) + postFix;
}
