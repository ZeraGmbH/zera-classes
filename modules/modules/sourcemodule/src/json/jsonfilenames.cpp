#include "jsonfilenames.h"
#include <QDir>
#include <QJsonDocument>

QString JsonFilenames::getJsonStructurePath(SupportedSourceTypes type)
{
    QString fileName, filePath;
    fileName = getJsonFileName(type);
    if(!fileName.isEmpty()) {
        filePath = QStringLiteral("://deviceinfo/") + fileName;
    }
    return filePath;
}

QString JsonFilenames::getJsonStatePath(SupportedSourceTypes type)
{
    QString fileName = getJsonFileName(type);
    QString statePath = getJsonStateDir();
    createDirIfNotExist(statePath);
    return statePath + fileName;
}

QString JsonFilenames::getJsonStatePath(QString deviceName, QString deviceVersion)
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

QString JsonFilenames::getJsonFileName(SupportedSourceTypes type)
{
    QString fileName;
    switch(type) {
    case SOURCE_MT_COMMON:
        fileName = QStringLiteral("3ph.json");
        break;
    case SOURCE_MT_CURRENT_ONLY:
        fileName = QStringLiteral("3ph-current-only.json");
        break;
    case SOURCE_DEMO_FG_4PHASE:
        fileName = QStringLiteral("4ph-harmonics.json");
        break;
    case SOURCE_MT_SINGLE_PHASE_ONLY:
        fileName = QStringLiteral("1ph.json");
        break;

    case SOURCE_TYPE_COUNT:
        qCritical("Use SOURCE_TYPE_COUNT for loop count only!!");
        break;
    }
    return fileName;
}

QString JsonFilenames::getJsonSourcesConnectedFileName()
{
    QString statePath = getJsonStateDir();
    createDirIfNotExist(statePath);
    return statePath + "sources-connected.json";
}

QString JsonFilenames::getJsonStateDir()
{
    QString statePath(ZC_DEV_STATE_PATH);
    if(!statePath.endsWith("/")) {
        statePath += "/";
    }
    return statePath;
}

void JsonFilenames::createDirIfNotExist(QString path)
{
    QDir dir;
    dir.mkpath(path);
}
