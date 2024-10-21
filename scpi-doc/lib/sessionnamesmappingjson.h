#ifndef SESSIONNAMESMAPPINGJSON_H
#define SESSIONNAMESMAPPINGJSON_H

#include <QJsonObject>
#include <QString>

class SessionNamesMappingJson
{
public:
    SessionNamesMappingJson(QString modmanConfigFile);
    bool storeMappedJsonFile(QString destFile);
    int getSessionCount(QString device);
private:
    QJsonObject createSessionNamesMappingJson(QString device);
    QString m_modmanConfigFile;
    QJsonObject m_mappedJson;
};

#endif // SESSIONNAMESMAPPINGJSON_H
