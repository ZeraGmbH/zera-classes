#include "sourcedevicestatus.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

namespace SOURCEMODULE
{

cSourceDeviceStatus::cSourceDeviceStatus()
{
    reset();
}

const QJsonObject &cSourceDeviceStatus::jsonStatus()
{
    return m_jsonStatus;
}

void cSourceDeviceStatus::reset()
{
    QString deviceInfoFileName = QStringLiteral("://deviceinfo/DefaultDeviceStatus.json");
    QFile deviceInfoFile(deviceInfoFileName);
    deviceInfoFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QByteArray jsondeviceInfoData = deviceInfoFile.readAll();
    deviceInfoFile.close();

    m_jsonStatus = QJsonDocument::fromJson(jsondeviceInfoData).object();
}

void cSourceDeviceStatus::setBusy(bool busy)
{
    m_jsonStatus["busy"] = busy;
}

void cSourceDeviceStatus::addError(const QString error)
{
    m_jsonStatus["errors"].toArray().append(error);
}

void cSourceDeviceStatus::addWarning(const QString warning)
{
    m_jsonStatus["warnings"].toArray().append(warning);
}

} //namespace SOURCEMODULE
