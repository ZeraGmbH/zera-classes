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
    QString configFileNameFull = QDir::cleanPath(m_configDirName + "/" + configFile);
    QString tmpNameString = moduleObject.value("name").toString();
    QByteArray xmlConfigData;
    if(!configFile.isEmpty()) {
        QFile tmpXmlConfigFile(configFileNameFull);
        if(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            xmlConfigData = tmpXmlConfigFile.readAll();
            tmpXmlConfigFile.close();
        }
        else {
            qCritical() << "Error opening config file for module:" << tmpNameString << "path:" << configFileNameFull;
            return;
        }
    }
    int moduleEntityId = moduleObject.value("id").toInt();
    emit sigLoadModule(tmpNameString, configFileNameFull, xmlConfigData, moduleEntityId);
}
