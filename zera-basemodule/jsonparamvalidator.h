#ifndef CJSONVALIDATORNOSCHEMA_H
#define CJSONVALIDATORNOSCHEMA_H

#include "paramvalidator.h"
class cZeraJsonParamsStructure;
class cZeraJsonParamsState;

class cJsonParamValidator : public cParamValidator
{
public:
    cJsonParamValidator();
    virtual bool isValidParam(QVariant &newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj)  override;
    void setJSonParameterState(cZeraJsonParamsStructure *pParamStructure);
private:
    cZeraJsonParamsStructure *m_pParamStructure = nullptr;
};

#endif // CJSONVALIDATORNOSCHEMA_H
