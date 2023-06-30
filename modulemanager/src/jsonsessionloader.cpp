#include "jsonsessionloader.h"
#include <zera-jsonfileloader.h>
#include <QFile>
#include <QJsonArray>
#include <QDebug>

JsonSessionLoader::JsonSessionLoader(QObject *t_Parent) :
    QObject(t_Parent)
{
}

const QString &JsonSessionLoader::getSessionFilePath() const
{
    return m_currentSessionFile;
}

void JsonSessionLoader::loadSession(QString t_filePath)
{
    QJsonParseError jsonError;
    qInfo() << "loading session:" << t_filePath;
    QJsonObject rootObj = cJsonFileLoader::loadJsonFile(t_filePath, &jsonError);
    if(!rootObj.isEmpty()) {
        if(jsonError.error == QJsonParseError::NoError) {
            QJsonValue tmpModules = rootObj.value("modules");
            if(tmpModules.isArray()) {
                for(auto entry : tmpModules.toArray()) {
                    if(entry.isObject()) {
                        parseModule(entry.toObject());
                    }
                }
            }
            else {
                qWarning() << "Error parsing session file / invalid content:" << t_filePath;
            }
        }
        else {
            qWarning() << "Error parsing session file:" << t_filePath << "error:" << jsonError.errorString();
        }
    }
    else {
        qWarning() << "Error opening session file:" << t_filePath;
    }
}

void JsonSessionLoader::parseModule(QJsonObject moduleObject)
{
    QString tmpNameString, tmpConfigPath, tmpConfigFilePath;
    int moduleId;
    QFile tmpXmlConfigFile;
    QByteArray xmlConfigData;

    tmpNameString = moduleObject.value("name").toString();
    tmpConfigPath = QString(MODMAN_CONFIG_PATH);
    if(!tmpConfigPath.endsWith("/")) {
        tmpConfigPath += "/";
    }
    tmpConfigFilePath = tmpConfigPath + moduleObject.value("configFile").toString();
    moduleId = moduleObject.value("id").toInt();

    tmpXmlConfigFile.setFileName(tmpConfigFilePath);
    if(tmpXmlConfigFile.exists() && tmpXmlConfigFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
        xmlConfigData = tmpXmlConfigFile.readAll();
        tmpXmlConfigFile.close();
        emit sigLoadModule(tmpNameString, tmpConfigFilePath, xmlConfigData, moduleId);
    }
    else {
        qWarning() << "Error opening config file for module:" << tmpNameString << "path:" << tmpConfigFilePath;
    }
}
