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

tSourceActionList cSourceActionSorter::SortActionMap(const tSourceActionMap sourceActionMap, const QList<cSourceActionTypes::ActionTypes> sortToFrontList)
{
    tSourceActionList sortedList;

    return sortedList;
}
