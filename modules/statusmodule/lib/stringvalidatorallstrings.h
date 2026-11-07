#ifndef STRINGVALIDATORALLSTRINGS_H
#define STRINGVALIDATORALLSTRINGS_H

#include <validatorinterface.h>

class StringValidatorAllStrings : public ValidatorInterface
{
public:
    bool isValidParam(QVariant &newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
};

#endif // STRINGVALIDATORALLSTRINGS_H
