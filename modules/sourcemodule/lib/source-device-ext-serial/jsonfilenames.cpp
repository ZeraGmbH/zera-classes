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

QString JsonFilenames::getJsonFileName(SupportedSourceTypes type)
{
    Q_INIT_RESOURCE(source_device_info);
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

