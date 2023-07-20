#include <QJsonArray>
#include <QJsonObject>
#include "intvalidator.h"

cIntValidator::cIntValidator(qint64 min, qint64 max, qint64 delta)
    :m_nMin(min), m_nMax(max), m_nDelta(delta)
{
}

cIntValidator::cIntValidator(const cIntValidator &ref)
{
    m_nMin = ref.m_nMin;
    m_nMax = ref.m_nMax;
    m_nDelta = ref.m_nDelta;
}

bool cIntValidator::isValidParam(QVariant &newValue)
{
    bool ok;
    qint32 value = newValue.toInt(&ok);
    ok = ok && (value <= m_nMax) && (value >= m_nMin);
    if(ok)
        newValue = QVariant::fromValue<qint32>(value);
    return ok;
}

void cIntValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "INTEGER");
    QJsonArray jsonArr = {m_nMin, m_nMax, m_nDelta};
    jsObj.insert("Data", jsonArr);
}


