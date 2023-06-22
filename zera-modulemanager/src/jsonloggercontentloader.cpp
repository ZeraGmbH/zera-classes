#include "jsonloggercontentloader.h"
#include <zera-jsonfileloader.h>
#include <QDir>
#include <QJsonArray>

JsonLoggerContentLoader::JsonLoggerContentLoader()
{
}

void JsonLoggerContentLoader::setConfigFileDir(const QString &dir)
{
    m_configFileDir = dir;
}

void JsonLoggerContentLoader::setSession(const QString &session)
{
    m_currentJsonContentSet = cJsonFileLoader::loadJsonFile(QDir::cleanPath(m_configFileDir + QDir::separator() + session));
}

QStringList JsonLoggerContentLoader::getAvailableContentSets()
{
    return m_currentJsonContentSet.keys();
}

QMap<int, QStringList> JsonLoggerContentLoader::getEntityComponents(const QString &contentSetName)
{
    QMap<int, QStringList> ret;
    const auto ecArr = m_currentJsonContentSet[contentSetName].toArray();
    for(const auto &arrEntry : ecArr) {
        int entityId = arrEntry["EntityId"].toInt();
        QStringList componentList;
        const QJsonArray arrComponents = arrEntry["Components"].toArray();
        for(const auto &component : arrComponents) {
            componentList.append(component.toString());
        }
        ret.insert(entityId, componentList);
    }
    return ret;
}
