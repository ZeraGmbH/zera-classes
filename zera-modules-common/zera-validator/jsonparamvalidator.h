#ifndef CJSONVALIDATORNOSCHEMA_H
#define CJSONVALIDATORNOSCHEMA_H

#include "validatorinterface.h"
#include <zera-json-params-state.h>

class cJsonParamValidator : public ValidatorInterface
{
public:
    cJsonParamValidator();
    void setJSonParameterStructure(const QJsonObject& jsonParamStructure);
    virtual bool isValidParam(QVariant &newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj)  override;
private:
    ZeraJsonParamsState m_paramState;
};

#endif // CJSONVALIDATORNOSCHEMA_H
