#include <QDir>
#include <QJsonDocument>
#include <zera-json-params-structure.h>
#include <zera-json-params-state.h>
#include <zera-jsonfileloader.h>

#include "sourcepersistentjsonstate.h"
#include "sourcejsonapi.h"

SourcePersistentJsonState::SourcePersistentJsonState(SupportedSourceTypes type) :
    m_sourceType(type),
    m_jsonParamStructure(cSourceJsonStructureLoader::getJsonStructure(type))
{
}

QJsonObject SourcePersistentJsonState::getJsonStructure()
{
    return m_jsonParamStructure;
}

QJsonObject SourcePersistentJsonState::loadJsonState()
{
    QJsonObject paramState;
    cZeraJsonParamsState jsonParamsState;
    jsonParamsState.setStructure(m_jsonParamStructure);
    // try to load persistent state file and validate it
    QJsonObject jsonStateFromFile =
            cJsonFileLoader::loadJsonFile(cSourceJsonFilenames::getJsonStatePathName(m_sourceType));
    cZeraJsonParamsState::ErrList errList = jsonParamsState.validateJsonState(jsonStateFromFile);
    if(errList.isEmpty()) {
        paramState = jsonStateFromFile;
    }
    if(paramState.isEmpty()) {
        // State either not there or corrupt: Heal ourselves and create
        // sane default state file
        paramState = jsonParamsState.createDefaultJsonState();
        saveJsonState(paramState);
    }
    // Override on state
    cSourceJsonParamApi paramApi;
    paramApi.setParams(paramState);
    paramApi.setOn(false);
    return paramApi.getParams();
}

void SourcePersistentJsonState::saveJsonState(QJsonObject state)
{
    QFile deviceStateFile(cSourceJsonFilenames::getJsonStatePathName(m_sourceType));
    if(deviceStateFile.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(state);
        deviceStateFile.write(doc.toJson(QJsonDocument::Indented));
        deviceStateFile.close();
    }
    else {
        qWarning("Default state file %s could not be written", qPrintable(deviceStateFile.fileName()));
    }
}
