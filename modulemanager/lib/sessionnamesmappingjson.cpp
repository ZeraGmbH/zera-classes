#include "sessionnamesmappingjson.h"
#include <zera-jsonfileloader.h>
#include "zera-json-merge.h"
#include <QJsonArray>

QJsonObject SessionNamesMappingJson::createSessionNamesMappingJsonAllDevices(QString modmanConfigFile)
{
    QJsonObject mappedJsonCom = createSessionNamesMappingJson(modmanConfigFile, "com5003");
    QJsonObject mappedJsonMt = createSessionNamesMappingJson(modmanConfigFile, "mt310s2");
    cJSONMerge::mergeJson(mappedJsonCom, mappedJsonMt);
    return mappedJsonCom;
}

QJsonObject SessionNamesMappingJson::createSessionNamesMappingJson(QString modmanConfigFile, QString device)
{
    QJsonObject jsonConfig = cJsonFileLoader::loadJsonFile(modmanConfigFile).value(device).toObject();
    QJsonArray availableSessions = jsonConfig["availableSessions"].toArray();
    QJsonArray sessionDisplayStrings = jsonConfig["sessionDisplayStrings"].toArray();

    QJsonObject mappedJson, sessions;
    for(int i = 0; i < availableSessions.count(); i++) {
        sessions.insert(availableSessions.at(i).toString(), sessionDisplayStrings.at(i));
    }
    mappedJson[device] = sessions;
    return mappedJson;
}
