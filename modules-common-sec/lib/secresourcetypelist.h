#ifndef SECRESOURCETYPELIST_H
#define SECRESOURCETYPELIST_H

#include "secconfigdatacommon.h"
#include <QList>

class SecResourceTypeList
{
public:
    void addTypesFromConfig(QList<TRefInput> refInputList);
    void addTypesFromConfig(QStringList inputList);
    QStringList getResourceTypeList();
private:
    void addToListNoDoubles(QStringList resourceTypeListToAdd);
    bool found(QList<QString>& list, QString searched);
    QStringList m_resourceTypeList;
};

#endif // SECRESOURCETYPELIST_H
