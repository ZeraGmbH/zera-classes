#include "testsourceswitchjson.h"
#include <jsonstructapi.h>

AbstractSourceSwitchJsonPtr TestSourceSwitchJson::create(const QJsonObject &sourceCapabilities)
{
    return std::make_unique<TestSourceSwitchJson>(sourceCapabilities);
}

TestSourceSwitchJson::TestSourceSwitchJson(const QJsonObject &sourceCapabilities)
{
    JsonStructApi capApi(sourceCapabilities);
    m_loadpointCurrent.setParams(capApi.getParamStructure());
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
    m_loadpointCurrent = entry.loadpoint;
    emit sigSwitchFinished(ok, entry.switchId);
}
