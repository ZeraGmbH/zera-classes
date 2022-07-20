#ifndef REGEXVALIDATOR_H
#define REGEXVALIDATOR_H

#include "paramvalidator.h"

class cRegExValidator: public cParamValidator
{
public:
    cRegExValidator(QString regex);
    cRegExValidator(const cRegExValidator& ref);
    virtual bool isValidParam(QVariant& newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj) override;
    virtual void setValidator(QString regex);
private:
    QString m_sRegEx;
};


#endif // REGEXVALIDATOR_H

