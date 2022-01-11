#include "jsonstructapi.h"

JsonStructApi::JsonStructApi(QJsonObject paramStructure) :
    m_paramStructure(paramStructure)
{
}

QJsonObject JsonStructApi::getParamStructure() const
{
    return m_paramStructure;
}

QString JsonStructApi::getDeviceName() const
{
    return m_paramStructure["Name"].toString();
}

QString JsonStructApi::getDeviceVersion() const
{
    return m_paramStructure["Version"].toString();
}

int JsonStructApi::getCountUPhases() const
{
    return m_paramStructure["UPhaseMax"].toInt(0);
}

int JsonStructApi::getCountIPhases() const
{
    return m_paramStructure["IPhaseMax"].toInt(0);
}

QByteArray JsonStructApi::getIoPrefix() const
{
    return m_paramStructure["IoPrefix"].toString().toLatin1();
}

void JsonStructApi::setDeviceName(QString name)
{
    m_paramStructure["Name"] = name;
}

void JsonStructApi::setDeviceVersion(QString version)
{
    m_paramStructure["Version"] = version;
}
