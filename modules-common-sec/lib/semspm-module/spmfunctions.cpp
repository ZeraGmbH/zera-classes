#include "spmfunctions.h"
#include "unithelper.h"

const QHash<QString, double> &SpmFunctions::getUnitFactorHash() const
{
    return cUnitHelper::getPowerUnitFactorHash();
}
