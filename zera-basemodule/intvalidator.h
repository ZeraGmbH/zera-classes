#ifndef INTVALIDATOR_H
#define INTVALIDATOR_H

#include "paramvalidator.h"

class cIntValidator: public cParamValidator
{
public:
    cIntValidator(qint32 min, qint32 max, qint32 delta = 0);

    virtual bool isValidParam(QVariant newValue);
    virtual void exportMetaData(QJsonObject jsObj);

private:
    qint32 m_nMin;
    qint32 m_nMax;
    qint32 m_nDelta;
};

#endif // INTVALIDATOR_H
