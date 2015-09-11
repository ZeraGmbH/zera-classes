#include <QJsonArray>
#include <QJsonObject>

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


void cDoubleValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "DOUBLE");

    QJsonArray jsonArr = {m_fMin, m_fMax, m_fDelta};
    jsObj.insert("Data", jsonArr);
}

