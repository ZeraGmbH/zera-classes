#include <QJsonArray>

#include "intvalidator.h"


cIntValidator::cIntValidator(qint32 min, qint32 max, qint32 delta)
    :m_nMin(min), m_nMax(max), m_nDelta(delta)
{
}


bool cIntValidator::isValidParam(QVariant newValue)
{
    bool ok;
    qint32 value;

    value = newValue.toInt(&ok);

    return (ok && (value <= m_nMax) && (value >= m_nMin));
}


void cIntValidator::exportMetaData(QJsonObject jsObj)
{
    QJsonArray jsonSCPIArr;

    jsonSCPIArr.append(QString("INTEGER"));
    jsonSCPIArr.append(QString("%1;%2;%3").arg(m_nMin).arg(m_nMax).arg(m_nDelta));

    jsObj.insert("VALIDATION", jsonSCPIArr);
}


