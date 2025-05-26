#include "persistentjsonstate.h"
#include "jsonfilenames.h"
#include "jsonstructapi.h"

PersistentJsonState::PersistentJsonState(const QJsonObject deviceCapabilities)
{
    JsonStructApi structApi(deviceCapabilities);
    QString deviceName = structApi.getDeviceName();
    QString deviceVersion = structApi.getDeviceVersion();
    m_stateFileName = JsonFilenames::getJsonStatePath(deviceName, deviceVersion);
    m_jsonStatePersistenceHelper.setStateFilePath(m_stateFileName);
    m_jsonStatePersistenceHelper.setJsonParamStructure(deviceCapabilities);
}

QJsonObject PersistentJsonState::getJsonStructure() const
{
    return m_jsonStatePersistenceHelper.getJsonParamStructure();
}

JsonParamApi PersistentJsonState::loadJsonState()
{
    QJsonObject paramState = m_jsonStatePersistenceHelper.loadState();

    // Override on state -> OFF
    JsonParamApi paramApi;
    paramApi.setParams(paramState);
    paramApi.setOn(false);
    return paramApi;
}

void PersistentJsonState::saveJsonState(JsonParamApi state)
{
    if(!m_jsonStatePersistenceHelper.saveState(state.getParams())) {
        qWarning("Default state file %s could not be written", qPrintable(m_stateFileName));
    }
}
