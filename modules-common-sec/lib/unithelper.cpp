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

const QHash<QString, double> cUnitHelper::m_powerUnitFactors = {
    { "MW",   1000.0 },
    { "kW",   1.0 },
    { "W",    0.001 },
    { "MVar", 1000.0 },
    { "kVar", 1.0 },
    { "Var",  0.001 },
    { "MVA", 1000.0 },
    { "kVA", 1.0 },
    { "VA", 0.001 }
};

const QHash<QString, double> &cUnitHelper::getPowerUnitFactorHash()
{
    return m_powerUnitFactors;
}

const QHash<QString, double> cUnitHelper::m_energyUnitFactors = {
    { "MWh",   1000.0 },
    { "kWh",   1.0 },
    { "Wh",    0.001 },
    { "MVarh", 1000.0 },
    { "kVarh", 1.0 },
    { "Varh",  0.001 },
    { "MVAh", 1000.0 },
    { "kVAh", 1.0 },
    { "VAh", 0.001 }
};

const QHash<QString, double> &cUnitHelper::getEnergyUnitFactorHash()
{
    return m_energyUnitFactors;
}
