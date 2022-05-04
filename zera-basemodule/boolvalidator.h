#ifndef BOOLVALIDATOR_H
#define BOOLVALIDATOR_H

#include "paramvalidator.h"

class cBoolValidator: public cParamValidator
{
public:
    virtual bool isValidParam(QVariant& newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj) override;
};

#endif // BOOLVALIDATOR_H
