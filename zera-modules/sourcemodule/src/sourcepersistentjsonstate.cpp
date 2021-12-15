#include "sourcepersistentjsonstate.h"
#include "sourcejsonapi.h"

SourcePersistentJsonState::SourcePersistentJsonState(SupportedSourceTypes type, QString deviceName, QString deviceVersion)
{
    QJsonObject paramStructure = cSourceJsonStructureLoader::getJsonStructure(type);
    SourceJsonStructApi structureApi = SourceJsonStructApi(paramStructure);
    if(!deviceName.isEmpty()) {
        m_stateFileName = cSourceJsonFilenames::getJsonStatePath(deviceName, deviceVersion);
        structureApi.setDeviceName(deviceName);
    }
    else {
        m_stateFileName = cSourceJsonFilenames::getJsonStatePath(type);
    }
    m_jsonStatePersistenceHelper.setStateFilePath(m_stateFileName);
    m_jsonStatePersistenceHelper.setJsonParamStructure(paramStructure);
}

QJsonObject SourcePersistentJsonState::getJsonStructure()
{
    return m_jsonStatePersistenceHelper.getJsonParamStructure();
}

QJsonObject SourcePersistentJsonState::loadJsonState()
{
    QJsonObject paramState = m_jsonStatePersistenceHelper.loadState();

    // Override on state -> OFF
    SourceJsonParamApi paramApi;
    paramApi.setParams(paramState);
    paramApi.setOn(false);
    return paramApi.getParams();
}

void SourcePersistentJsonState::saveJsonState(QJsonObject state)
{
    if(!m_jsonStatePersistenceHelper.saveState(state)) {
        qWarning("Default state file %s could not be written", qPrintable(m_stateFileName));
    }
}

