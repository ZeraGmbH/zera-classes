#include "jsonsessionloader.h"
#include <zera-jsonfileloader.h>
#include <QFile>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

QString JsonSessionLoader::m_configDirName = QString(MODMAN_CONFIG_PATH);

JsonSessionLoader::JsonSessionLoader(QObject *parent) :
    QObject(parent)
{
}

const QString &JsonSessionLoader::getSessionFilePath() const
{
    return m_currentSessionFile;
}

void JsonSessionLoader::loadSession(QString filePath)
{
    QJsonParseError jsonError;
    qInfo() << "Loading session:" << filePath;
    QJsonObject rootObj = cJsonFileLoader::loadJsonFile(filePath, &jsonError);
    if(!rootObj.isEmpty()) {
        if(jsonError.error == QJsonParseError::NoError) {
            QJsonValue tmpModules = rootObj.value("modules");
            if(tmpModules.isArray()) {
                const QJsonArray jArr = tmpModules.toArray();
                qInfo("Session contains %i modules", jArr.count());
                for(const auto &entry : jArr) {
                    if(entry.isObject())
                        parseModule(entry.toObject());
                }
            }
            else
                qCritical() << "Error parsing session file / invalid content:" << filePath;
        }
        else
            qCritical() << "Error parsing session file:" << filePath << "error:" << jsonError.errorString();
    }
    else
        qCritical() << "Error opening session file:" << filePath;
}

void JsonSessionLoader::parseModule(QJsonObject moduleObject)
{
    QString configFile = moduleObject.value("configFile").toString();
    QString configFileNameFull;
    if(!configFile.isEmpty()) {
        if(configFile.startsWith(":/"))
            configFileNameFull = configFile;
        else
            configFileNameFull = QDir::cleanPath(m_configDirName + "/" + configFile);
    }
    QString moduleName = moduleObject.value("name").toString();
    int moduleEntityId = moduleObject.value("id").toInt();
    int moduleNum = moduleObject.value("module_num").toInt();
    emit sigLoadModule(moduleName, configFileNameFull, moduleEntityId, moduleNum);
}
