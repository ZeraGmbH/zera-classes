#include <QJsonArray>

#include "stringvalidator.h"


cStringValidator::cStringValidator(QString possibilities)
{
    m_sPossibilitiesList = possibilities.split(';');
}


cStringValidator::cStringValidator(QStringList possibilities)
    :m_sPossibilitiesList(possibilities)
{
}


bool cStringValidator::isValidParam(QVariant newValue)
{
    QString s;

    s = newValue.toString();
    return (m_sPossibilitiesList.contains(s));
}


void cStringValidator::exportMetaData(QJsonObject jsObj)
{
    QJsonArray jsonSCPIArr;

    jsonSCPIArr.append(QString("STRING"));
    jsonSCPIArr.append(m_sPossibilitiesList.join(';'));

    jsObj.insert("VALIDATION", jsonSCPIArr);
}


