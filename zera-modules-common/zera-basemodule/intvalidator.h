#ifndef INTVALIDATOR_H
#define INTVALIDATOR_H

#include "paramvalidator.h"

class cIntValidator: public cParamValidator
{
public:
    cIntValidator() { }
    cIntValidator(qint64 min, qint64 max, qint64 delta = 0);
    cIntValidator(const cIntValidator& ref);
    virtual bool isValidParam(QVariant& newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj) override;
private:
    qint64 m_nMin;
    qint64 m_nMax;
    qint64 m_nDelta;
};

#endif // INTVALIDATOR_H
