#include "sourcejsonapi.h"

SourceJsonStructApi::SourceJsonStructApi(QJsonObject paramStructure) :
    m_paramStructure(paramStructure)
{
}

QJsonObject SourceJsonStructApi::getParamStructure() const
{
    return m_paramStructure;
}

QString SourceJsonStructApi::getDeviceName() const
{
    return m_paramStructure["Name"].toString();
}

QString SourceJsonStructApi::getDeviceVersion() const
{
    return m_paramStructure["Version"].toString();
}

int SourceJsonStructApi::getCountUPhases() const
{
    return m_paramStructure["UPhaseMax"].toInt(0);
}

int SourceJsonStructApi::getCountIPhases() const
{
    return m_paramStructure["IPhaseMax"].toInt(0);
}

QByteArray SourceJsonStructApi::getIoPrefix() const
{
    return m_paramStructure["IoPrefix"].toString().toLatin1();
}

void SourceJsonStructApi::setDeviceName(QString name)
{
    m_paramStructure["Name"] = name;
}

void SourceJsonStructApi::setDeviceVersion(QString version)
{
    m_paramStructure["Version"] = version;
}

SourceJsonParamApi::SourceJsonParamApi()
{
}

QJsonObject SourceJsonParamApi::getParams() const
{
    return m_params;
}

void SourceJsonParamApi::setParams(QJsonObject params)
{
    m_params = params;
}

bool SourceJsonParamApi::getOn() const
{
    return m_params.contains("on") && m_params["on"].toBool();
}

void SourceJsonParamApi::setOn(bool on)
{
    m_params["on"] = on;
}

double SourceJsonParamApi::getRms(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["rms"].toDouble(0.0);
}

double SourceJsonParamApi::getAngle(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["angle"].toDouble(0.0);
}

bool SourceJsonParamApi::getOn(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject obj = m_params[phaseName].toObject();
    return obj.contains("on") && obj["on"].toBool();
}

bool SourceJsonParamApi::getFreqVarOn() const
{
    return m_params["Frequency"].toObject()["type"] == "var";
}

double SourceJsonParamApi::getFreqVal() const
{
    return m_params["Frequency"].toObject()["val"].toDouble();
}

QString SourceJsonParamApi::getPhaseName(phaseType type, int phaseIdxBase0) const
{
    return QString("%1%2").arg(getPhaseNamePrefix(type)).arg(phaseIdxBase0+1);
}

QString SourceJsonParamApi::getPhaseNamePrefix(phaseType type) const
{
    switch(type) {
    case phaseType::U:
        return "U";
    case phaseType::I:
        return "I";
    }
    return QString();
}
