#include "testsourceswitchjson.h"
#include <jsonstructapi.h>
#include <zera-json-params-state.h>
#include <zera-json-params-structure.h>

std::shared_ptr<TestSourceSwitchJson> TestSourceSwitchJson::create(const QJsonObject &jsonStruct)
{
    return std::make_shared<TestSourceSwitchJson>(jsonStruct);
}

TestSourceSwitchJson::TestSourceSwitchJson(const QJsonObject &jsonStruct)
{
    ZeraJsonParamsStructure jsonParamsStructParser;
    jsonParamsStructParser.setJson(jsonStruct);
    QJsonObject structExpanded = jsonParamsStructParser.getJson();

    JsonStructApi capApi(structExpanded);
    QJsonObject structApi = capApi.getParamStructure();

    ZeraJsonParamsState state(structApi);
    m_loadpointCurrent.setParams(state.getDefaultJsonState());
}

int TestSourceSwitchJson::switchState(const JsonParamApi &paramState)
{
    m_currentId++;
    m_pendingLoadpoints.enqueue({m_currentId, paramState});
    return m_currentId;
}

JsonParamApi TestSourceSwitchJson::getCurrLoadpoint()
{
    return m_loadpointCurrent;
}

void TestSourceSwitchJson::fireFinish(bool ok)
{
    QueueEntry entry = m_pendingLoadpoints.dequeue();
    if (ok)
        m_loadpointCurrent = entry.loadpoint;
    emit sigSwitchFinished(ok, entry.switchId);
}
