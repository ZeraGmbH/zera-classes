#include "jsonloggercontentsessionloader.h"
#include <zera-jsonfileloader.h>
#include <QDir>
#include <QJsonArray>

JsonLoggerContentSessionLoader::JsonLoggerContentSessionLoader()
{
}

void JsonLoggerContentSessionLoader::setConfigFileDir(const QString &dir)
{
    m_configFileDir = dir;
}

void JsonLoggerContentSessionLoader::setSession(const QString &session)
{
    m_currentJsonContentSet = cJsonFileLoader::loadJsonFile(QDir::cleanPath(m_configFileDir + QDir::separator() + session));
}

QStringList JsonLoggerContentSessionLoader::getAvailableContentSets()
{
    QStringList ret;
    if(!m_currentJsonContentSet["modules"].toArray().isEmpty()) {
        ret.append("ZeraAll");
    }
    return ret;
}

QMap<int, QStringList> JsonLoggerContentSessionLoader::getEntityComponents(const QString &contentSetName)
{
    QMap<int, QStringList> ret;
    if(!getAvailableContentSets().isEmpty() && contentSetName == "ZeraAll") {
        const auto ecArr = m_currentJsonContentSet["modules"].toArray();
        for(const auto &arrEntry : ecArr) {
            int entityId = arrEntry["id"].toInt();
            ret.insert(entityId, QStringList());
        }
    }
    return ret;
}
