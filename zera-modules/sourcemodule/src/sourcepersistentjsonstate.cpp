#include "sourcepersistentjsonstate.h"
#include "sourcejsonapi.h"

SourcePersistentJsonState::SourcePersistentJsonState(SupportedSourceTypes type) :
    m_stateFileName(cSourceJsonFilenames::getJsonStatePathName(type))
{
    m_jsonStatePersistenceHelper.setStateFilePath(m_stateFileName);
    m_jsonStatePersistenceHelper.setJsonParamStructure(cSourceJsonStructureLoader::getJsonStructure(type));
}

QJsonObject SourcePersistentJsonState::getJsonStructure()
{
    return m_jsonStatePersistenceHelper.getJsonParamStructure();
}

QJsonObject SourcePersistentJsonState::loadJsonState()
{
    QJsonObject paramState = m_jsonStatePersistenceHelper.loadState();

    // Override on state -> OFF
    cSourceJsonParamApi paramApi;
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

