#include "apimodulerequestvalidator.h"

namespace APIMODULE
{
cApiModuleRequestValidator::cApiModuleRequestValidator() {

    QJsonObject zjParams;
    QJsonObject name;
    name.insert("type", "string");
    zjParams.insert("name", name);

    QJsonObject type;
    type.insert("type", "string");
    zjParams.insert("type", type);

    QJsonObject token;
    token.insert("type", "string");
    zjParams.insert("token", token);

    m_pendingRequestParams.insert("zj_params", zjParams);

    setJSonParameterStructure(m_pendingRequestParams);
}

bool cApiModuleRequestValidator::isValidParam(QVariant &newValue)
{
    bool valid = cJsonParamValidator::isValidParam(newValue);
    if (valid)
        newValue = newValue.toJsonObject();
    return valid;
}

}
