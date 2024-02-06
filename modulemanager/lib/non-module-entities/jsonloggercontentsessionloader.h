#ifndef JSONLOGGERCONTENTSESSIONLOADER_H
#define JSONLOGGERCONTENTSESSIONLOADER_H

#include <vf_loggercontenthandler.h>
#include <QJsonObject>

class JsonLoggerContentSessionLoader : public LoggerContentHandler
{
public:
    JsonLoggerContentSessionLoader();
    void setConfigFileDir(const QString &dir) override;
    void setSession(const QString &session) override;
    QStringList getAvailableContentSets() override;
    QMap<int, QStringList> getEntityComponents(const QString &contentSetName) override;
private:
    QString m_configFileDir;
    QJsonObject m_currentJsonContentSet;
};

#endif // JSONLOGGERCONTENTSESSIONLOADER_H
