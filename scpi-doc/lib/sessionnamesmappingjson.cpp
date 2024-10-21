#include "sessionnamesmappingjson.h"
#include <zera-jsonfileloader.h>
#include <zera-json-merge.h>
#include <QJsonArray>
#include <QStringList>

SessionNamesMappingJson::SessionNamesMappingJson(QString modmanConfigFile) :
    m_modmanConfigFile(modmanConfigFile)
{
    QJsonObject deviceJson = createSessionNamesMappingJson("mt310s2");
    cJSONMerge::mergeJson(m_mappedJson, deviceJson);
    deviceJson = createSessionNamesMappingJson("com5003");
    cJSONMerge::mergeJson(m_mappedJson, deviceJson);
}

bool SessionNamesMappingJson::storeMappedJsonFile(QString destFile)
{
    return cJsonFileLoader::storeJsonFile(destFile, m_mappedJson);
}

int SessionNamesMappingJson::getSessionCount(QString device)
{
    return m_mappedJson[device].toObject().count();
}

QString SessionNamesMappingJson::getSessionNameForExternalUsers(QString internalSessionName)
{
    QString externalName;
    foreach(const QString& device, m_mappedJson.keys()) {
        QJsonObject sessions = m_mappedJson.value(device).toObject();
        if (sessions.contains(internalSessionName)) {
            externalName = sessions.value(internalSessionName).toString();
            break;
        }
    }
    return externalName;
}

QJsonObject SessionNamesMappingJson::createSessionNamesMappingJson(QString device)
{
    QJsonObject jsonConfig = cJsonFileLoader::loadJsonFile(m_modmanConfigFile).value(device).toObject();
    QJsonArray availableSessions = jsonConfig["availableSessions"].toArray();
    QJsonArray sessionDisplayStrings = jsonConfig["sessionDisplayStrings"].toArray();

    QJsonObject mappedJson, sessions;
    for(int i = 0; i < availableSessions.count(); i++) {
        sessions.insert(availableSessions.at(i).toString(), sessionDisplayStrings.at(i));
    }
    mappedJson[device] = sessions;
    return mappedJson;
}
