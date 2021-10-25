#ifndef CJSONVALIDATORNOSCHEMA_H
#define CJSONVALIDATORNOSCHEMA_H

#include "paramvalidator.h"
#include <zera-json-params-state.h>

class cJsonParamValidator : public cParamValidator
{
public:
    cJsonParamValidator();
    void setJSonParameterStructure(const QJsonObject& jsonParamStructure);
    virtual bool isValidParam(QVariant &newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj)  override;
private:
    cZeraJsonParamsState m_paramState;
};

#endif // CJSONVALIDATORNOSCHEMA_H
