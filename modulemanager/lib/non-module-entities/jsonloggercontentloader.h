#ifndef JSONLOGGERCONTENTLOADER_H
#define JSONLOGGERCONTENTLOADER_H

#include <vf_loggercontenthandler.h>
#include <QJsonObject>

class JsonLoggerContentLoader : public LoggerContentHandler
{
public:
    JsonLoggerContentLoader();
    void setConfigFileDir(const QString &dir) override;
    void setSession(const QString &session) override;
    QStringList getAvailableContentSets() override;
    QMap<int, QStringList> getEntityComponents(const QString &contentSetName) override;
private:
    QString m_configFileDir;
    QJsonObject m_currentJsonContentSet;
};

#endif // JSONLOGGERCONTENTLOADER_H
