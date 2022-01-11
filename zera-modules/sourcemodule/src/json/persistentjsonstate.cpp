#include "persistentjsonstate.h"
#include "jsonstructapi.h"
#include "jsonparamapi.h"
#include "jsonfilenames.h"
#include "jsonstructureloader.h"

PersistentJsonState::PersistentJsonState(SupportedSourceTypes type, QString deviceName, QString deviceVersion)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, deviceVersion);
    if(deviceName.isEmpty()) {
        JsonStructApi structApi(paramStructure);
        deviceName = structApi.getDeviceName();
    }
    m_stateFileName = JsonFilenames::getJsonStatePath(deviceName, deviceVersion);
    m_jsonStatePersistenceHelper.setStateFilePath(m_stateFileName);
    m_jsonStatePersistenceHelper.setJsonParamStructure(paramStructure);
}

QJsonObject PersistentJsonState::getJsonStructure() const
{
    return m_jsonStatePersistenceHelper.getJsonParamStructure();
}

QJsonObject PersistentJsonState::loadJsonState()
{
    QJsonObject paramState = m_jsonStatePersistenceHelper.loadState();

    // Override on state -> OFF
    JsonParamApi paramApi;
    paramApi.setParams(paramState);
    paramApi.setOn(false);
    return paramApi.getParams();
}

void PersistentJsonState::saveJsonState(QJsonObject state)
{
    if(!m_jsonStatePersistenceHelper.saveState(state)) {
        qWarning("Default state file %s could not be written", qPrintable(m_stateFileName));
    }
}

