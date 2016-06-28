#include <QJsonArray>
#include <QJsonObject>

#include "intvalidator.h"


cIntValidator::cIntValidator(qint64 min, qint64 max, qint64 delta)
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


void cIntValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "INTEGER");

    QJsonArray jsonArr = {m_nMin, m_nMax, m_nDelta};
    jsObj.insert("Data", jsonArr);
}


