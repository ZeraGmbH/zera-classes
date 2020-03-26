#ifndef REGEXVALIDATOR_H
#define REGEXVALIDATOR_H

#include "paramvalidator.h"

class cRegExValidator: public cParamValidator
{
public:
    cRegExValidator(){}
    cRegExValidator(QString regex);
    cRegExValidator(const cRegExValidator& ref);
    virtual ~cRegExValidator(){}

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);
    virtual void setValidator(QString regex);

private:
    QString m_sRegEx;
};


#endif // REGEXVALIDATOR_H

