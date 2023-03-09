#ifndef BOOLVALIDATOR_H
#define BOOLVALIDATOR_H

#include "validatorinterface.h"

class cBoolValidator: public ValidatorInterface
{
public:
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
};

#endif // BOOLVALIDATOR_H
