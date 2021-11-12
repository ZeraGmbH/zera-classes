#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
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
    return QRegularExpression(m_sRegEx).match(newValue.toString()).hasMatch();
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

