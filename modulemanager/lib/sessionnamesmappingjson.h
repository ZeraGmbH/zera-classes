#ifndef SESSIONNAMESMAPPINGJSON_H
#define SESSIONNAMESMAPPINGJSON_H

#include <QJsonObject>
#include <QString>

class SessionNamesMappingJson
{
public:
    explicit SessionNamesMappingJson(const QString &modmanConfigFile);
    bool storeMappedJsonFile(const QString &destFile);
    int getSessionCount(const QString &device);
    QString getSessionNameForExternalUsers(const QString &internalSessionName);
private:
    QJsonObject createSessionNamesMappingJson(const QString &device);
    QString m_modmanConfigFile;
    QJsonObject m_mappedJson;
};

#endif // SESSIONNAMESMAPPINGJSON_H
