#include <QJsonArray>
#include <QJsonObject>

#include "stringvalidator.h"


cStringValidator::cStringValidator(QString possibilities)
{
    m_sPossibilitiesList = possibilities.split(';');
}


cStringValidator::cStringValidator(QStringList possibilities)
    :m_sPossibilitiesList(possibilities)
{
}


bool cStringValidator::isValidParam(QVariant& newValue)
{
    QString s;

    s = newValue.toString();
    return (m_sPossibilitiesList.contains(s));
}


void cStringValidator::exportMetaData(QJsonObject& jsObj)
{
    QJsonArray jsonArr;

    jsObj.insert("Type", "STRING");

    int n;
    n = m_sPossibilitiesList.count();
    for (int i = 0; i < n; i++)
        jsonArr.append(m_sPossibilitiesList.at(i));
    jsObj.insert("Data", jsonArr);
}


