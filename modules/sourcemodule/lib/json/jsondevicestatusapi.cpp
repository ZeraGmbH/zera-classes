#include "jsondevicestatusapi.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariant>
#include <QStringList>

JsonDeviceStatusApi::JsonDeviceStatusApi()
{
    reset();
}

void JsonDeviceStatusApi::reset()
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

void JsonDeviceStatusApi::clearWarningsErrors()
{
    QJsonArray emptyArr;
    m_jsonStatus[keyErrors] = emptyArr;
    m_jsonStatus[keyWarnings] = emptyArr;
}

void JsonDeviceStatusApi::setBusy(bool busy)
{
    m_jsonStatus[keyBusy] = busy;
}

void JsonDeviceStatusApi::addError(const QString error)
{
    appendToArray(keyErrors, error);
}

void JsonDeviceStatusApi::addWarning(const QString warning)
{
    appendToArray(keyWarnings, warning);
}

void JsonDeviceStatusApi::setDeviceInfo(const QString strDeviceInfo)
{
    m_jsonStatus[keyDeviceInfo] = strDeviceInfo;
}

const QJsonObject &JsonDeviceStatusApi::getJsonStatus()
{
    return m_jsonStatus;
}

bool JsonDeviceStatusApi::getBusy() const
{
    return m_jsonStatus[keyBusy].toBool();
}

QStringList JsonDeviceStatusApi::getErrors() const
{
    return getArray(keyErrors);
}

QStringList JsonDeviceStatusApi::getWarnings() const
{
    return getArray(keyWarnings);
}

QString JsonDeviceStatusApi::getDeviceInfo() const
{
    return m_jsonStatus[keyDeviceInfo].toString();
}

QStringList JsonDeviceStatusApi::getArray(QString key) const
{
    QStringList strList;
    auto arr = m_jsonStatus[key].toArray();
    const auto variantList = arr.toVariantList();
    for (const auto &entry : variantList) {
        strList.append(entry.toString());
    }
    return strList;
}

void JsonDeviceStatusApi::appendToArray(QString key, QString value)
{
    QJsonArray arr = m_jsonStatus[key].toArray();
    arr.append(value);
    m_jsonStatus[key] = arr;
}
