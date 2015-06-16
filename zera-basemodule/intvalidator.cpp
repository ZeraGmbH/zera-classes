#include "intvalidator.h"


cIntValidator::cIntValidator(qint32 min, qint32 max)
    :m_nMin(min), m_nMax(max)
{
}


bool cIntValidator::isValidParam(QVariant newValue)
{
    bool ok;
    qint32 value;

    value = newValue.toInt(&ok);

    return (ok && (value <= m_nMax) && (value >= m_nMin));
}


