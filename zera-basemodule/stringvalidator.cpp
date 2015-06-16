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
