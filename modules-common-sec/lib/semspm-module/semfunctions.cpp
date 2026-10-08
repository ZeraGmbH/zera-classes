#include "semfunctions.h"
#include "unithelper.h"

const QHash<QString, double> &SemFunctions::getUnitFactorHash() const
{
    return cUnitHelper::getEnergyUnitFactorHash();
}
