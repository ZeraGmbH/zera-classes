#ifndef SECRESOURCETYPELIST_H
#define SECRESOURCETYPELIST_H

#include "secconfigdatacommon.h"
#include <QList>

class SecResourceTypeList
{
public:
    void addTypesFromConfig(const TSecCommonReferenceConfigs &refConfig);
    void addTypesFromConfig(const TSecCommonDutConfigs &dutConfig);
    const QStringList &getResourceTypeList() const;
private:
    void addTypesFromList(const QStringList &inputList);
    void addToListNoDoubles(const QStringList &resourceTypeListToAdd);
    static bool found(const QList<QString>& list, QString searched);
    QStringList m_resourceTypeList;
};

#endif // SECRESOURCETYPELIST_H
