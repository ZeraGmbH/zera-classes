#include <QDir>
#include <QJsonDocument>
#include <zera-json-params-structure.h>
#include <zera-json-params-state.h>
#include <zera-jsonfileloader.h>
#include "supportedsources.h"
#include "sourcejsonapi.h"

QString cSourceJsonFilenames::getJsonStructurePathName(SupportedSourceTypes type)
{
    QString fileName, filePath;
    fileName = getJsonFileName(type);
    if(!fileName.isEmpty()) {
        filePath = QStringLiteral("://deviceinfo/") + fileName;
    }
    return filePath;
}

QString cSourceJsonFilenames::getJsonStatePathName(SupportedSourceTypes type)
{
    QString fileName = getJsonFileName(type);
    QString statePath(ZC_DEV_STATE_PATH);
    if(!statePath.endsWith("/")) {
        statePath += "/";
    }
    QDir dir; // make sure path exists
    dir.mkpath(statePath);
    fileName = statePath + fileName;
    return fileName;
}

QString cSourceJsonFilenames::getJsonFileName(SupportedSourceTypes type)
{
    QString fileName;
    switch(type) {
    case SOURCE_MT_COMMON:
        fileName = QStringLiteral("MT3000.json");
        break;
    case SOURCE_MT400_20:
        fileName = QStringLiteral("MT400-20.json");
        break;
    case SOURCE_DEMO_FG_4PHASE:
        fileName = QStringLiteral("FG4PhaseTest.json");
        break;
    case SOURCE_MT507:
        fileName = QStringLiteral("MT507.json");
        break;

    case SOURCE_TYPE_COUNT:
        qCritical("Use SOURCE_TYPE_COUNT for loop count only!!");
        break;
    }
    return fileName;
}

QJsonObject cSourceJsonStructureLoader::getJsonStructure(QString fileName)
{
    QJsonObject jsonStructureRaw = cJsonFileLoader::loadJsonFile(fileName);
    cZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.loadStructure(jsonStructureRaw);
    return jsonParamsStructure.jsonStructure();
}


QJsonObject cSourceJsonStructureLoader::getJsonStructure(SupportedSourceTypes type)
{
    return getJsonStructure(cSourceJsonFilenames::getJsonStructurePathName(type));
}


cSourcePersistentJsonState::cSourcePersistentJsonState(SupportedSourceTypes type) :
    m_sourceType(type),
    m_jsonParamStructure(cSourceJsonStructureLoader::getJsonStructure(type))
{
}

QJsonObject cSourcePersistentJsonState::getJsonStructure()
{
    return m_jsonParamStructure;
}

QJsonObject cSourcePersistentJsonState::loadJsonState()
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

void cSourcePersistentJsonState::saveJsonState(QJsonObject state)
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

