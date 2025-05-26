#include "jsonstatefilenames.h"
#include <QDir>

QString JsonStateFilenames::getJsonStatePath(const QString &deviceName, const QString &deviceVersion)
{
    QString fileName = deviceName;
    if(!deviceVersion.isEmpty())
        fileName += "-" + deviceVersion;
    fileName += ".json";
    QString statePath = getJsonStateDir();
    createDirIfNotExist(statePath);
    return statePath + fileName;
}

QString JsonStateFilenames::getJsonStateDir()
{
    QString statePath(ZC_DEV_STATE_PATH);
    if(!statePath.endsWith("/"))
        statePath += "/";
    return statePath;
}

void JsonStateFilenames::createDirIfNotExist(const QString &path)
{
    QDir dir;
    dir.mkpath(path);
}
