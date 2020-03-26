#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>

#include "regexvalidator.h"


cRegExValidator::cRegExValidator(QString regex)
{
    m_sRegEx = regex;
}


cRegExValidator::cRegExValidator(const cRegExValidator &ref)
{
    m_sRegEx = ref.m_sRegEx;
}


bool cRegExValidator::isValidParam(QVariant& newValue)
{
    QString s;

    s = newValue.toString();
    s.remove(QRegExp(m_sRegEx));
    return (s.length() == 0);
}


void cRegExValidator::exportMetaData(QJsonObject& jsObj)
{
    jsObj.insert("Type", "REGEX");
    jsObj.insert("Data", m_sRegEx);
}


void cRegExValidator::setValidator(QString regex)
{
    m_sRegEx = regex;
}



