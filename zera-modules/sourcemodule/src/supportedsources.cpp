#include <QDir>
#include <QJsonDocument>
#include <zera-json-params-structure.h>
#include <zera-json-params-state.h>
#include <zera-jsonfileloader.h>
#include "supportedsources.h"
#include "sourcejsonapi.h"

QString cSourceJsonFilenames::getJsonStructurePath(SupportedSourceTypes type)
{
    QString fileName, filePath;
    fileName = getJsonFileName(type);
    if(!fileName.isEmpty()) {
        filePath = QStringLiteral("://deviceinfo/") + fileName;
    }
    return filePath;
}

QString cSourceJsonFilenames::getJsonStateDir()
{
    QString statePath(ZC_DEV_STATE_PATH);
    if(!statePath.endsWith("/")) {
        statePath += "/";
    }
    return statePath;
}

QString cSourceJsonFilenames::getJsonStatePath(SupportedSourceTypes type)
{
    QString fileName = getJsonFileName(type);
    QString statePath = getJsonStateDir();
    createDirIfNotExist(statePath);
    return statePath + fileName;
}

QString cSourceJsonFilenames::getJsonStatePath(QString deviceName, QString deviceVersion)
{
    QString fileName = deviceName;
    if(!deviceVersion.isEmpty()) {
        fileName += "-" + deviceVersion;
    }
    fileName += ".json";
    QString statePath = getJsonStateDir();
    createDirIfNotExist(statePath);
    return statePath + fileName;
}

QString cSourceJsonFilenames::getJsonFileName(SupportedSourceTypes type)
{
    QString fileName;
    switch(type) {
    case SOURCE_MT_COMMON:
        fileName = QStringLiteral("MT3000.json");
        break;
    case SOURCE_MT_CURRENT_ONLY:
        fileName = QStringLiteral("MT400-20.json");
        break;
    case SOURCE_DEMO_FG_4PHASE:
        fileName = QStringLiteral("FG4PhaseTest.json");
        break;
    case SOURCE_MT_SINGLE_PHASE_ONLY:
        fileName = QStringLiteral("MT507.json");
        break;

    case SOURCE_TYPE_COUNT:
        qCritical("Use SOURCE_TYPE_COUNT for loop count only!!");
        break;
    }
    return fileName;
}

void cSourceJsonFilenames::createDirIfNotExist(QString path)
{
    QDir dir;
    dir.mkpath(path);
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
    return getJsonStructure(cSourceJsonFilenames::getJsonStructurePath(type));
}


