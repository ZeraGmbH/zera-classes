#ifndef MODULEMANAGERCONFIG_H
#define MODULEMANAGERCONFIG_H

#include <QJsonObject>
#include <mutex>

class ModulemanagerConfig
{
public:
    static ModulemanagerConfig *getInstance();
    static void setDemoDevice(QString demoDevice);
    static QString getConfigFileName();
    bool isValid();
    const QString getDeviceName();
    bool getCustomerDataEnabled();
    const QStringList getAvailableSessions();
    const QString getDefaultSession();
    void setDefaultSession(QString session);
    bool containsDeviceName(QString devName);
protected:
    static QString m_configFileName;
private:
    ModulemanagerConfig();
    const QString getDevNameFromUBoot();
    QJsonObject getDeviceJson();
    void setDeviceJson(QJsonObject devJson);
    void save();
    static ModulemanagerConfig *m_instance;
    static std::once_flag m_onceflag;
    static QString m_demoDevice;
    QJsonObject m_jsonConfig;
    QString m_deviceName;
};

#endif // MODULEMANAGERCONFIG_H
