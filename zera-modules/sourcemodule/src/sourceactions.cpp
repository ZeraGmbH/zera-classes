#include "sourceactions.h"

cSourceAction::cSourceAction(QJsonObject jsonSourceParamStructure, QJsonObject jsonSourceParamState) :
    m_jsonSourceParamStructure(jsonSourceParamStructure),
    m_jsonSourceParamState(jsonSourceParamState)
{
}

tActionMap cSourceActionGenerator::GenerateActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState)
{
    tActionMap map;
    for(int type=0; type<int(cSourceAction::ACTION_COUNT); ++type) {
        cSourceAction action(jsonSourceParamStructure, jsonSourceParamState);
        map.insert(cSourceAction::ActionTypes(type), action);
    }
    return map;
}

tPeriodicActionMap cSourceActionGenerator::GeneratePeriodicActionMap(const QJsonObject jsonSourceParamStructure, const QJsonObject jsonSourceParamState)
{
    tPeriodicActionMap map;
    for(int type=0; type<int(cSourceAction::PERIODIC_ACTION_COUNT); ++type) {
        cSourceAction action(jsonSourceParamStructure, jsonSourceParamState);
        map.insert(cSourceAction::PeriodicActionTypes(type), action);
    }
    return map;
}
