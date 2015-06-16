#ifndef BOOLVALIDATOR_H
#define BOOLVALIDATOR_H

#include "paramvalidator.h"

class cBoolValidator: public cParamValidator
{
    cBoolValidator(){}

    virtual bool isValidParam(QVariant newValue);
};

#endif // BOOLVALIDATOR_H
