#include "testsourceswitchjson.h"

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

JsonParamApi TestSourceSwitchJson::getLoadpointRequestedLast()
{
    return m_loadpointRequestedLast;
}

void TestSourceSwitchJson::fireFinish(bool ok)
{
    QueueEntry entry = m_pendingLoadpoints.dequeue();
    m_loadpointCurrent = entry.loadpoint;
    emit sigSwitchFinished(ok, entry.switchId);
}
