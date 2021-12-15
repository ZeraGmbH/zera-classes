#include "sourcejsonapi.h"

SourceJsonStructApi::SourceJsonStructApi(QJsonObject paramStructure) :
    m_paramStructure(paramStructure)
{
}

QJsonObject SourceJsonStructApi::getParamStructure()
{
    return m_paramStructure;
}

QString SourceJsonStructApi::getDeviceName()
{
    return m_paramStructure["Name"].toString();
}

int SourceJsonStructApi::getCountUPhases()
{
    return m_paramStructure["UPhaseMax"].toInt(0);
}

int SourceJsonStructApi::getCountIPhases()
{
    return m_paramStructure["IPhaseMax"].toInt(0);
}

QByteArray SourceJsonStructApi::getIoPrefix()
{
    return m_paramStructure["IoPrefix"].toString().toLatin1();
}

void SourceJsonStructApi::setDeviceName(QString name)
{
    m_paramStructure["Name"] = name;
}

SourceJsonParamApi::SourceJsonParamApi()
{
}

QJsonObject SourceJsonParamApi::getParams()
{
    return m_params;
}

void SourceJsonParamApi::setParams(QJsonObject params)
{
    m_params = params;
}

bool SourceJsonParamApi::getOn()
{
    return m_params.contains("on") && m_params["on"].toBool();
}

void SourceJsonParamApi::setOn(bool on)
{
    m_params["on"] = on;
}

double SourceJsonParamApi::getRms(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["rms"].toDouble(0.0);
}

double SourceJsonParamApi::getAngle(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["angle"].toDouble(0.0);
}

bool SourceJsonParamApi::getOn(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject obj = m_params[phaseName].toObject();
    return obj.contains("on") && obj["on"].toBool();
}

bool SourceJsonParamApi::getFreqVarOn()
{
    return m_params["Frequency"].toObject()["type"] == "var";
}

double SourceJsonParamApi::getFreqVal()
{
    return m_params["Frequency"].toObject()["val"].toDouble();
}

QString SourceJsonParamApi::getPhaseName(phaseType type, int phaseIdxBase0)
{
    return QString("%1%2").arg(getPhaseNamePrefix(type)).arg(phaseIdxBase0+1);
}

QString SourceJsonParamApi::getPhaseNamePrefix(phaseType type)
{
    switch(type) {
    case phaseType::U:
        return "U";
    case phaseType::I:
        return "I";
    }
    return QString();
}
