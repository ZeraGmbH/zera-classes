#include <QJsonArray>
#include <QJsonObject>
#include "doublevalidator.h"

cDoubleValidator::cDoubleValidator(double min, double max, double delta)
    :m_fMin(min), m_fMax(max), m_fDelta(delta)
{
}

cDoubleValidator::cDoubleValidator(const cDoubleValidator &ref)
{
    m_fMin = ref.m_fMin;
    m_fMax = ref.m_fMax;
    m_fDelta = ref.m_fDelta;
}

double cDoubleValidator::getMinValue()
{
    return m_fMin;
}

double cDoubleValidator::getMaxValue()
{
    return m_fMax;
}

double cDoubleValidator::getDelta()
{
    return m_fDelta;
}

bool cDoubleValidator::isValidParam(QVariant& newValue)
{
    bool ok;
    double value = newValue.toDouble(&ok);
    return (ok && (value <= m_fMax) && (value >= m_fMin));
}

void cDoubleValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "DOUBLE");
    QJsonArray jsonArr = {m_fMin, m_fMax, m_fDelta};
    jsObj.insert("Data", jsonArr);
}
