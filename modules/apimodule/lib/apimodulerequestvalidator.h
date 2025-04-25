#ifndef APIMODULEREQUESTVALIDATOR_H
#define APIMODULEREQUESTVALIDATOR_H

#include <validatorinterface.h>

namespace APIMODULE
{
class cApiModuleRequestValidator : public ValidatorInterface
{
public:
    cApiModuleRequestValidator();
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
};
}
#endif // APIMODULEREQUESTVALIDATOR_H
