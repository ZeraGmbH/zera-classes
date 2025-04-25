#ifndef APIMODULEREQUESTVALIDATOR_H
#define APIMODULEREQUESTVALIDATOR_H

#include <jsonparamvalidator.h>

namespace APIMODULE
{
class cApiModuleRequestValidator : public cJsonParamValidator
{
public:
    cApiModuleRequestValidator();
    bool isValidParam(QVariant& newValue) override;
private:
    QJsonObject m_pendingRequestParams;
};
}
#endif // APIMODULEREQUESTVALIDATOR_H
