#include "sourcejsonapi.h"

cSourceJsonStructApi::cSourceJsonStructApi(QJsonObject paramStructure) :
    m_paramStructure(paramStructure)
{
}

int cSourceJsonStructApi::getCountUPhases()
{
    return m_paramStructure["UPhaseMax"].toInt(0);
}

int cSourceJsonStructApi::getCountIPhases()
{
    return m_paramStructure["IPhaseMax"].toInt(0);
}

QByteArray cSourceJsonStructApi::getIoPrefix()
{
    return m_paramStructure["IoPrefix"].toString().toLatin1();
}

cSourceJsonParamApi::cSourceJsonParamApi()
{
}

QJsonObject cSourceJsonParamApi::getParams()
{
    return m_params;
}

void cSourceJsonParamApi::setParams(QJsonObject params)
{
    m_params = params;
}

bool cSourceJsonParamApi::getOn()
{
    return m_params.contains("on") && m_params["on"].toBool();
}

void cSourceJsonParamApi::setOn(bool on)
{
    m_params["on"] = on;
}

double cSourceJsonParamApi::getRms(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["rms"].toDouble(0.0);
}

double cSourceJsonParamApi::getAngle(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    return m_params[phaseName].toObject()["angle"].toDouble(0.0);
}

bool cSourceJsonParamApi::getOn(phaseType type, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(type, phaseIdxBase0);
    QJsonObject obj = m_params[phaseName].toObject();
    return obj.contains("on") && obj["on"].toBool();
}

bool cSourceJsonParamApi::getFreqVarOn()
{
    return m_params["Frequency"].toObject()["type"] == "var";
}

double cSourceJsonParamApi::getFreqVal()
{
    return m_params["Frequency"].toObject()["val"].toDouble();
}

QString cSourceJsonParamApi::getPhaseName(phaseType type, int phaseIdxBase0)
{
    if(type == phaseType::U){
        return QString("%1%2").arg("U").arg(phaseIdxBase0+1);
    }else if(type == phaseType::I){
        return QString("%1%2").arg("I").arg(phaseIdxBase0+1);
    }
    return QString();
}
