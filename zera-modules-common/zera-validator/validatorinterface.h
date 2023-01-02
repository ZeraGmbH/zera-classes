#ifndef VALIDATORINTERFACE_H
#define VALIDATORINTERFACE_H

#include <QVariant>
#include <QJsonObject>

// pure virtual class for parameter validation

class ValidatorInterface
{
public:
    virtual bool isValidParam(QVariant& newValue) = 0;
    virtual void exportMetaData(QJsonObject& jsObj) = 0;
    virtual ~ValidatorInterface() = default;
};


#endif // VALIDATORINTERFACE_H
