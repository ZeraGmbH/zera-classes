#ifndef MODULEMANAGERCONFIG_H
#define MODULEMANAGERCONFIG_H

#include <QJsonObject>
#include <mutex>

class ModulemanagerConfig
{
public:
    static ModulemanagerConfig *getInstance();
    bool isValid();
    const QString getDeviceName();
    bool getCustomerDataEnabled();
    const QStringList getAvailableSessions();
    const QString getDefaultSession();
    void setDefaultSession(QString session);
private:
    ModulemanagerConfig();
    const QString getDevNameFromUBoot();
    QJsonObject getDeviceJson();
    void setDeviceJson(QJsonObject devJson);
    void save();
    static ModulemanagerConfig *m_instance;
    static std::once_flag m_onceflag;
    QJsonObject m_jsonConfig;
    QString m_deviceName;
};

#endif // MODULEMANAGERCONFIG_H
