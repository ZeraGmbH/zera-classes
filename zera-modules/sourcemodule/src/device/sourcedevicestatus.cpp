#include "sourcedevicestatus.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariant>
#include <QStringList>

SourceDeviceStatus::SourceDeviceStatus()
{
    reset();
}

void SourceDeviceStatus::reset()
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
static const QString keyDeviceInfo = "deviceinfo";

void SourceDeviceStatus::clearWarningsErrors()
{
    QJsonArray emptyArr;
    m_jsonStatus[keyErrors] = emptyArr;
    m_jsonStatus[keyWarnings] = emptyArr;
}

void SourceDeviceStatus::setBusy(bool busy)
{
    m_jsonStatus[keyBusy] = busy;
}

void SourceDeviceStatus::addError(const QString error)
{
    appendToArray(keyErrors, error);
}

void SourceDeviceStatus::addWarning(const QString warning)
{
    appendToArray(keyWarnings, warning);
}

void SourceDeviceStatus::setDeviceInfo(const QString strDeviceInfo)
{
    m_jsonStatus[keyDeviceInfo] = strDeviceInfo;
}

const QJsonObject &SourceDeviceStatus::getJsonStatus()
{
    return m_jsonStatus;
}

bool SourceDeviceStatus::getBusy()
{
    return m_jsonStatus[keyBusy].toBool();
}

QStringList SourceDeviceStatus::getErrors()
{
    return getArray(keyErrors);
}

QStringList SourceDeviceStatus::getWarnings()
{
    return getArray(keyWarnings);
}

QString SourceDeviceStatus::getDeviceInfo()
{
    return m_jsonStatus[keyDeviceInfo].toString();
}

QStringList SourceDeviceStatus::getArray(QString key)
{
    QStringList strList;
    auto arr = m_jsonStatus[key].toArray();
    auto variantList = arr.toVariantList();
    for (auto entry : variantList) {
        strList.append(entry.toString());
    }
    return strList;
}

void SourceDeviceStatus::appendToArray(QString key, QString value)
{
    QJsonArray arr = m_jsonStatus[key].toArray();
    arr.append(value);
    m_jsonStatus[key] = arr;
}
