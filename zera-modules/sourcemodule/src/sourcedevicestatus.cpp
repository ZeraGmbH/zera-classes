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

void cSourceDeviceStatus::reset()
{
    QString deviceInfoFileName = QStringLiteral("://devicestatus/DefaultDeviceStatus.json");
    QFile deviceInfoFile(deviceInfoFileName);
    deviceInfoFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QByteArray jsondeviceInfoData = deviceInfoFile.readAll();
    deviceInfoFile.close();

    m_jsonStatus = QJsonDocument::fromJson(jsondeviceInfoData).object();
}

static const QString keyBusy = "busy";
static const QString keyErrors = "errors";
static const QString keyWarnings = "warnings";

void cSourceDeviceStatus::setBusy(bool busy)
{
    m_jsonStatus[keyBusy] = busy;
}

void cSourceDeviceStatus::addError(const QString error)
{
    appendToArray(keyErrors, error);
}

void cSourceDeviceStatus::addWarning(const QString warning)
{
    appendToArray(keyWarnings, warning);
}

const QJsonObject &cSourceDeviceStatus::getJsonStatus()
{
    return m_jsonStatus;
}

bool cSourceDeviceStatus::getBusy()
{
    return m_jsonStatus[keyBusy].toBool();
}

QStringList cSourceDeviceStatus::getErrors()
{
    return getArray(keyErrors);
}

QStringList cSourceDeviceStatus::getWarnings()
{
    return getArray(keyWarnings);
}

QStringList cSourceDeviceStatus::getArray(QString key)
{
    QStringList strList;
    auto arr = m_jsonStatus[key].toArray();
    auto variantList = arr.toVariantList();
    for (auto entry : variantList) {
        strList.append(entry.toString());
    }
    return strList;
}

void cSourceDeviceStatus::appendToArray(QString key, QString value)
{
    QJsonArray arr = m_jsonStatus[key].toArray();
    arr.append(value);
    m_jsonStatus[key] = arr;
}

} //namespace SOURCEMODULE
