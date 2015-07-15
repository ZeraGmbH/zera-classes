#include <QJsonArray>

#include "doublevalidator.h"


cDoubleValidator::cDoubleValidator(double min, double max, double delta)
    :m_fMin(min), m_fMax(max), m_fDelta(delta)
{
}


bool cDoubleValidator::isValidParam(QVariant newValue)
{
    bool ok;
    double value;

    value = newValue.toDouble(&ok);

    return (ok && (value <= m_fMax) && (value >= m_fMin));
}


void cDoubleValidator::exportMetaData(QJsonObject jsObj)
{
    QJsonArray jsonSCPIArr;

    jsonSCPIArr.append(QString("DOUBLE"));
    jsonSCPIArr.append(QString("%1;%2;%3").arg(m_fMin).arg(m_fMax).arg(m_fDelta));

    jsObj.insert("VALIDATION", jsonSCPIArr);
}

