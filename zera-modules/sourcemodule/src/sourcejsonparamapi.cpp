#include "sourcejsonparamapi.h"

cSourceJsonStructureApi::cSourceJsonStructureApi(QJsonObject paramStructure) :
    m_paramStructure(paramStructure)
{
}

int cSourceJsonStructureApi::getCountUPhases()
{
    return m_paramStructure["UPhaseMax"].toInt(0);
}

int cSourceJsonStructureApi::getCountIPhases()
{
    return m_paramStructure["IPhaseMax"].toInt(0);
}

QByteArray cSourceJsonStructureApi::getIoPrefix()
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

double cSourceJsonParamApi::getRms(bool u, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(u, phaseIdxBase0);
    return m_params[phaseName].toObject()["rms"].toDouble(0.0);
}

double cSourceJsonParamApi::getAngle(bool u, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(u, phaseIdxBase0);
    return m_params[phaseName].toObject()["angle"].toDouble(0.0);
}

bool cSourceJsonParamApi::getOn(bool u, int phaseIdxBase0)
{
    QString phaseName = getPhaseName(u, phaseIdxBase0);
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

QString cSourceJsonParamApi::getPhaseName(bool u, int phaseIdxBase0)
{
    return QString("%1%2").arg(u ? "U" : "I").arg(phaseIdxBase0+1);
}
