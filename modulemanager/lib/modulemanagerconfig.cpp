#include "modulemanagerconfig.h"
#include <zera-jsonfileloader.h>
#include <zenuxdeviceinfo.h>
#include <QDir>
#include <QJsonArray>
#include <QFile>
#include <QVariant>
#include <QDebug>

ModulemanagerConfig* ModulemanagerConfig::m_instance = nullptr;
std::once_flag ModulemanagerConfig::m_onceflag;
QString ModulemanagerConfig::m_deviceName;
QString ModulemanagerConfig::m_configFileName = MODMAN_DEFAULT_SESSION;
QString ModulemanagerConfig::m_configFileDir = MODMAN_CONFIG_PATH;

ModulemanagerConfig *ModulemanagerConfig::getInstance()
{
    std::call_once(m_onceflag, [&]() { m_instance = new ModulemanagerConfig(); });
    return m_instance;
}

void ModulemanagerConfig::setDemoDevice(QString demoDevice)
{
    m_deviceName = demoDevice;
    m_configFileName = MODMAN_DEFAULT_SESSION;
}

QString ModulemanagerConfig::getConfigFileNameFull()
{
    return QDir::cleanPath(m_configFileDir + "/" + m_configFileName);
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

const QStringList ModulemanagerConfig::getSessionDisplayStrings()
{
    QJsonObject devJson = getDeviceJson();
    const QVariant tmpAvailList = devJson.value("sessionDisplayStrings").toArray().toVariantList();
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

bool ModulemanagerConfig::containsDeviceName(QString devName)
{
    if(m_jsonConfig.contains(devName))
        return true;
    return false;
}

static const char* topTagConnnection = "connectivity";

NetworkConnectionInfo ModulemanagerConfig::getPcbConnectionInfo()
{
    const char* serviceTag = "pcbService";
    QJsonObject serviceObj = m_jsonConfig[topTagConnnection].toObject()[serviceTag].toObject();
    return NetworkConnectionInfo(
        serviceObj["ip"].toString(),
        serviceObj["port"].toInt());
}

NetworkConnectionInfo ModulemanagerConfig::getDspConnectionInfo()
{
    const char* serviceTag = "dspService";
    QJsonObject serviceObj = m_jsonConfig[topTagConnnection].toObject()[serviceTag].toObject();
    return NetworkConnectionInfo(
        serviceObj["ip"].toString(),
        serviceObj["port"].toInt());
}

NetworkConnectionInfo ModulemanagerConfig::getSecConnectionInfo()
{
    const char* serviceTag = "secService";
    QJsonObject serviceObj = m_jsonConfig[topTagConnnection].toObject()[serviceTag].toObject();
    return NetworkConnectionInfo(
        serviceObj["ip"].toString(),
        serviceObj["port"].toInt());
}

NetworkConnectionInfo ModulemanagerConfig::getResmanConnectionInfo()
{
    const char* serviceTag = "resmanService";
    QJsonObject serviceObj = m_jsonConfig[topTagConnnection].toObject()[serviceTag].toObject();
    return NetworkConnectionInfo(
        serviceObj["ip"].toString(),
        serviceObj["port"].toInt());
}

bool ModulemanagerConfig::isDevMode()
{
    return m_jsonConfig["devMode"].toBool();
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
    cJsonFileLoader::storeJsonFile(getConfigFileNameFull(), m_jsonConfig);
}

ModulemanagerConfig::ModulemanagerConfig()
{
    m_jsonConfig = cJsonFileLoader::loadJsonFile(getConfigFileNameFull());
    if(m_deviceName.isEmpty()) {
        m_deviceName = ZenuxDeviceInfo::getDeviceNameFromKernelParam();
        if(m_deviceName.isEmpty() && isValid()) {
            m_deviceName = m_jsonConfig["deviceName"].toString();
        }
    }
}

