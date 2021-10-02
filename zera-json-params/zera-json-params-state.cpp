#include "zera-json-params-state.h"
#include "zera-json-params-structure.h"
#include "zera-json-merge.h"

cZeraJsonParamsState::cZeraJsonParamsState()
{
}

bool cZeraJsonParamsState::isValid()
{
    return !m_jsonObjState.isEmpty();
}

void cZeraJsonParamsState::setState(const QJsonObject& jsonState)
{
    m_jsonObjState = jsonState;
}

void cZeraJsonParamsState::mergeState(const QJsonObject &jsonState)
{
    cJSONMerge::mergeJson(m_jsonObjState, jsonState);
}

const QJsonObject& cZeraJsonParamsState::state()
{
    return m_jsonObjState;
}

