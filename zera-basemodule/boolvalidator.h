#ifndef BOOLVALIDATOR_H
#define BOOLVALIDATOR_H

#include "paramvalidator.h"

class cBoolValidator: public cParamValidator
{
public:
    cBoolValidator(){}

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);
};

#endif // BOOLVALIDATOR_H
