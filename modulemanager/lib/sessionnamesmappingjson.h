#ifndef SESSIONNAMESMAPPINGJSON_H
#define SESSIONNAMESMAPPINGJSON_H

#include <QJsonObject>
#include <QString>

class SessionNamesMappingJson
{
public:
    static QJsonObject createSessionNamesMappingJsonAllDevices(QString modmanConfigFile);
    static QJsonObject createSessionNamesMappingJson(QString modmanConfigFile, QString device);
};

#endif // SESSIONNAMESMAPPINGJSON_H
