#include "sourceactions.h"

cSourceAction::cSourceAction()
{
}

cSourceAction::cSourceAction(QJsonObject jsonSourceParamStructure, QJsonObject jsonSourceParamState) :
    m_jsonSourceParamStructure(jsonSourceParamStructure),
    m_jsonSourceParamState(jsonSourceParamState)
{
}

tSourceActionMap cSourceActionGenerator::GenerateLoadActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState)
{
    tSourceActionMap map;
    for(int type=0; type<cSourceActionTypes::getLoadActionTypeCount(); ++type) {
        cSourceAction action(jsonSourceParamStructure, jsonSourceParamState);
        map.insert(cSourceActionTypes::ActionTypes(type), action);
    }
    return map;
}

tSourceActionMap cSourceActionGenerator::GeneratePeriodicActionMap(const QJsonObject jsonSourceParamStructure)
{
    tSourceActionMap map;
    for(int type=cSourceActionTypes::PERIODIC_FIRST; type<int(cSourceActionTypes::PERIODIC_LAST); ++type) {
        cSourceAction action(jsonSourceParamStructure, QJsonObject());
        map.insert(cSourceActionTypes::ActionTypes(type), action);
    }
    return map;
}

tSourceActionList cSourceActionSorter::SortActionMap(tSourceActionMap sourceActionMap, const QList<cSourceActionTypes::ActionTypes> sortToFrontList)
{
    tSourceActionList sortedList;
    for(auto sortFrontEntry : sortToFrontList) { // append front sort
        auto iter = sourceActionMap.find(sortFrontEntry);
        if(iter != sourceActionMap.end()) {
            tSourceListEntry listEntry;
            listEntry.action = iter.value();
            listEntry.actionType = iter.key();
            sortedList.append(listEntry);
            sourceActionMap.erase(iter);
        }
    }
    for(auto iter = sourceActionMap.begin(); iter != sourceActionMap.end(); ++iter) { // append remaining
        tSourceListEntry listEntry;
        listEntry.action = iter.value();
        listEntry.actionType = iter.key();
        sortedList.append(listEntry);
    }
    return sortedList;
}
