#ifndef PARAMVALIDATOR_H
#define PARAMVALIDATOR_H

#include <QVariant>

// pure virtual class for parameter validation

class cParamValidator
{
public:
    cParamValidator(){}

    virtual bool isValidParam(QVariant newValue) = 0;
};


#endif // PARAMVALIDATOR_H
