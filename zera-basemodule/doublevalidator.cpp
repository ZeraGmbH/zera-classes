#include "doublevalidator.h"


cDoubleValidator::cDoubleValidator(double min, double max)
    :m_fMin(min), m_fMax(max)
{
}


bool cDoubleValidator::isValidParam(QVariant newValue)
{
    bool ok;
    double value;

    value = newValue.toDouble(&ok);

    return (ok && (value <= m_fMax) && (value >= m_fMin));
}

