#include "jsondevicestatus.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariant>
#include <QStringList>

JsonDeviceStatus::JsonDeviceStatus()
{
    reset();
}

void JsonDeviceStatus::reset()
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

void JsonDeviceStatus::clearWarningsErrors()
{
    QJsonArray emptyArr;
    m_jsonStatus[keyErrors] = emptyArr;
    m_jsonStatus[keyWarnings] = emptyArr;
}

void JsonDeviceStatus::setBusy(bool busy)
{
    m_jsonStatus[keyBusy] = busy;
}

void JsonDeviceStatus::addError(const QString error)
{
    appendToArray(keyErrors, error);
}

void JsonDeviceStatus::addWarning(const QString warning)
{
    appendToArray(keyWarnings, warning);
}

void JsonDeviceStatus::setDeviceInfo(const QString strDeviceInfo)
{
    m_jsonStatus[keyDeviceInfo] = strDeviceInfo;
}

const QJsonObject &JsonDeviceStatus::getJsonStatus()
{
    return m_jsonStatus;
}

bool JsonDeviceStatus::getBusy()
{
    return m_jsonStatus[keyBusy].toBool();
}

QStringList JsonDeviceStatus::getErrors()
{
    return getArray(keyErrors);
}

QStringList JsonDeviceStatus::getWarnings()
{
    return getArray(keyWarnings);
}

QString JsonDeviceStatus::getDeviceInfo()
{
    return m_jsonStatus[keyDeviceInfo].toString();
}

QStringList JsonDeviceStatus::getArray(QString key)
{
    QStringList strList;
    auto arr = m_jsonStatus[key].toArray();
    auto variantList = arr.toVariantList();
    for (auto entry : variantList) {
        strList.append(entry.toString());
    }
    return strList;
}

void JsonDeviceStatus::appendToArray(QString key, QString value)
{
    QJsonArray arr = m_jsonStatus[key].toArray();
    arr.append(value);
    m_jsonStatus[key] = arr;
}
