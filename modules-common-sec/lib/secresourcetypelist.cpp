#include "secresourcetypelist.h"

void SecResourceTypeList::addTypesFromConfig(QList<TRefInput> refInputList)
{
    QStringList inputNames;
    for(const auto &input : refInputList)
        inputNames.append(input.inputName);
    addTypesFromConfig(inputNames);
}

void SecResourceTypeList::addTypesFromConfig(QStringList inputList)
{
    QStringList resourceTypeListToAdd;
    // REF
    if (found(inputList, "fi"))
        resourceTypeListToAdd.append("FRQINPUT");
    if (found(inputList, "fo"))
        resourceTypeListToAdd.append("SOURCE");
    // DUT
    if (found(inputList, "sh"))
        resourceTypeListToAdd.append("SCHEAD");
    if (found(inputList, "hk"))
        resourceTypeListToAdd.append("HKEY");
    addToListNoDoubles(resourceTypeListToAdd);
}

QStringList SecResourceTypeList::getResourceTypeList()
{
    return m_resourceTypeList;
}

void SecResourceTypeList::addToListNoDoubles(QStringList resourceTypeListToAdd)
{
    for(const auto &resource : resourceTypeListToAdd) {
        if(!m_resourceTypeList.contains(resource))
            m_resourceTypeList.append(resource);
    }
}

bool SecResourceTypeList::found(QList<QString> &list, QString searched)
{
    for (int i = 0; i < list.count(); i++)
        if (list.at(i).contains(searched))
            return true;
    return false;
}
