#ifndef PARAMVALIDATOR_H
#define PARAMVALIDATOR_H

#include <QVariant>
#include <QJsonObject>

// pure virtual class for parameter validation

class cParamValidator
{
public:
    cParamValidator(){}

    virtual bool isValidParam(QVariant newValue) = 0;
    virtual void exportMetaData(QJsonObject& jsObj) = 0;
};


#endif // PARAMVALIDATOR_H
