#include "jsonsessionloader.h"
#include <zera-jsonfileloader.h>
#include <QFile>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

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
                qWarning() << "Error parsing session file / invalid content:" << filePath;
        }
        else
            qWarning() << "Error parsing session file:" << filePath << "error:" << jsonError.errorString();
    }
    else
        qWarning() << "Error opening session file:" << filePath;
}

void JsonSessionLoader::parseModule(QJsonObject moduleObject)
{
    QString configFileNameFull = QDir::cleanPath(QString(MODMAN_CONFIG_PATH) + "/" + moduleObject.value("configFile").toString());
    int moduleEntityId = moduleObject.value("id").toInt();
    QString tmpNameString = moduleObject.value("name").toString();
    QFile tmpXmlConfigFile(configFileNameFull);
    if(tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
        QByteArray xmlConfigData = tmpXmlConfigFile.readAll();
        tmpXmlConfigFile.close();
        emit sigLoadModule(tmpNameString, configFileNameFull, xmlConfigData, moduleEntityId);
    }
    else
        qWarning() << "Error opening config file for module:" << tmpNameString << "path:" << configFileNameFull;
}
