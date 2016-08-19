#ifndef INTVALIDATOR_H
#define INTVALIDATOR_H

#include "paramvalidator.h"

class cIntValidator: public cParamValidator
{
public:
    cIntValidator(qint64 min, qint64 max, qint64 delta = 0);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    qint64 m_nMin;
    qint64 m_nMax;
    qint64 m_nDelta;
};

#endif // INTVALIDATOR_H
