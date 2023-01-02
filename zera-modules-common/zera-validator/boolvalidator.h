#ifndef BOOLVALIDATOR_H
#define BOOLVALIDATOR_H

#include "validatorinterface.h"

class cBoolValidator: public ValidatorInterface
{
public:
    virtual bool isValidParam(QVariant& newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj) override;
};

#endif // BOOLVALIDATOR_H
