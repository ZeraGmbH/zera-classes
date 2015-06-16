#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "paramvalidator.h"

class cDoubleValidator: public cParamValidator
{
public:
    cDoubleValidator(double min, double max);

    virtual bool isValidParam(QVariant newValue);

private:
    double m_fMin;
    double m_fMax;
};


#endif // DOUBLEVALIDATOR_H
