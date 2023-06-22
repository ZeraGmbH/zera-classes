#include "modulemanagerconfig.h"
#include <zera-jsonfileloader.h>
#include <QJsonArray>
#include <QFile>
#include <QVariant>
#include <QDebug>

ModulemanagerConfig* ModulemanagerConfig::m_instance = nullptr;
std::once_flag ModulemanagerConfig::m_onceflag;

ModulemanagerConfig *ModulemanagerConfig::getInstance()
{
    std::call_once(m_onceflag, [&]() { m_instance = new ModulemanagerConfig(); });
    return m_instance;
}

bool ModulemanagerConfig::isValid()
{
    return !m_jsonConfig.isEmpty();
}

const QString ModulemanagerConfig::getDeviceName()
{
    return m_deviceName;
}

bool ModulemanagerConfig::getCustomerDataEnabled()
{
    QJsonObject devJson = getDeviceJson();
    QString entry = "customerdataSystemEnabled";
    return devJson.value(entry).toBool(false);
}

const QStringList ModulemanagerConfig::getAvailableSessions()
{
    QJsonObject devJson = getDeviceJson();
    const QVariant tmpAvailList = devJson.value("availableSessions").toArray().toVariantList();
    return tmpAvailList.toStringList();
}

const QString ModulemanagerConfig::getDefaultSession()
{
    QJsonObject devJson = getDeviceJson();
    return devJson.value("defaultSession").toString();
}

void ModulemanagerConfig::setDefaultSession(QString session)
{
    if(getDefaultSession() != session && getAvailableSessions().contains(session)) {
        QJsonObject devJson = getDeviceJson();
        devJson["defaultSession"] = session;
        setDeviceJson(devJson);
        save();
    }
}

const QString ModulemanagerConfig::getDevNameFromUBoot()
{
    QString strDeviceName;
    // Check for kernel cmdline param which u-boot should set
    QFile procFileCmdLine(QLatin1String("/proc/cmdline"));
    if(procFileCmdLine.open(QIODevice::ReadOnly)) {
        QString cmdLine = procFileCmdLine.readAll();
        procFileCmdLine.close();
        // Extract 'zera_device=<device_name>'
        QRegExp regExp(QLatin1String("\\bzera_device=[^ ]*"));
        if(regExp.indexIn(cmdLine) != -1) {
            strDeviceName = regExp.cap(0);
            // The following should go in regex above but...
            strDeviceName.replace(QLatin1String("zera_device="), QLatin1String(""));
            strDeviceName.replace(QLatin1String("\n"), QLatin1String(""));
            qInfo() << "ZERA Device from kernel cmdline: " << strDeviceName;
        }
    }
    return strDeviceName;
}

QJsonObject ModulemanagerConfig::getDeviceJson()
{
    return isValid() ? m_jsonConfig[m_deviceName].toObject() : QJsonObject();
}

void ModulemanagerConfig::setDeviceJson(QJsonObject devJson)
{
    m_jsonConfig[m_deviceName] = devJson;
}

void ModulemanagerConfig::save()
{
    cJsonFileLoader::storeJsonFile(MODMAN_CONFIG_FILE, m_jsonConfig);
}

ModulemanagerConfig::ModulemanagerConfig()
{
    m_deviceName = getDevNameFromUBoot();
    m_jsonConfig = cJsonFileLoader::loadJsonFile(MODMAN_CONFIG_FILE);
    if(m_deviceName.isEmpty() && isValid()) {
        m_deviceName = m_jsonConfig["deviceName"].toString();
    }
}
