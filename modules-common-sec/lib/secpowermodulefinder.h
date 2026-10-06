#ifndef SECPOWERMODULEFINDER_H
#define SECPOWERMODULEFINDER_H

#include <vs_abstractdatabase.h>

class SecPowerModuleFinder
{
public:
    static int findEntity(const QString &refName, const VeinStorage::AbstractDatabase *veinDb);
};

#endif // SECPOWERMODULEFINDER_H
