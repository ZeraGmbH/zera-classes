#ifndef INTVALIDATOR_H
#define INTVALIDATOR_H

#include "paramvalidator.h"

class cIntValidator: public cParamValidator
{
public:
    cIntValidator(qint32 min, qint32 max);

    virtual bool isValidParam(QVariant newValue);

private:
    qint32 m_nMin;
    qint32 m_nMax;
};

#endif // INTVALIDATOR_H
