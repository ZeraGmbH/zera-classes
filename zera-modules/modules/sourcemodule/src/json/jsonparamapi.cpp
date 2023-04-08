#include "jsonparamapi.h"

JsonParamApi::JsonParamApi()
{
}

QJsonObject JsonParamApi::getParams() const
{
    return m_params;
}

void JsonParamApi::setParams(QJsonObject params)
{
    m_params = params;
}

bool JsonParamApi::getOn() const
{
    return m_params.contains("on") && m_params["on"].toBool();
}

void JsonParamApi::setOn(bool on)
{
    m_params["on"] = on;
}

double JsonParamApi::getRms(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["rms"].toDouble(0.0);
}

double JsonParamApi::getAngle(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["angle"].toDouble(0.0);
}

bool JsonParamApi::getOn(phaseType type, int phaseIdxBase0) const
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject obj = m_params[phaseName].toObject();
    return obj.contains("on") && obj["on"].toBool();
}

bool JsonParamApi::getFreqVarOn() const
{
    return m_params["Frequency"].toObject()["type"] == "var";
}

double JsonParamApi::getFreqVal() const
{
    return m_params["Frequency"].toObject()["val"].toDouble();
}

QString JsonParamApi::getPhaseName(phaseType type, int phaseIdxBase0) const
{
    return QString("%1%2").arg(getPhaseNamePrefix(type)).arg(phaseIdxBase0+1);
}

QString JsonParamApi::getPhaseNamePrefix(phaseType type) const
{
    switch(type) {
    case phaseType::U:
        return "U";
    case phaseType::I:
        return "I";
    }
    return QString();
}
