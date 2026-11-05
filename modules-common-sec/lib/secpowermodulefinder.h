#ifndef SECPOWERMODULEFINDER_H
#define SECPOWERMODULEFINDER_H

#include "secconfigdatacommon.h"
#include <vs_abstractdatabase.h>

class SecPowerModuleFinder
{
public:
    static int findEntity(const QString &refName, const VeinStorage::AbstractDatabase *veinDb);
    static bool testConfiguredRefInputs(const TSecCommonReferenceConfigs &refConfigs,
                                        const VeinStorage::AbstractDatabase *veinDb);
};

#endif // SECPOWERMODULEFINDER_H
