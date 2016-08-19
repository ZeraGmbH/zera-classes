#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "paramvalidator.h"

class cDoubleValidator: public cParamValidator
{
public:
    cDoubleValidator(double min, double max, double delta);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    double m_fMin;
    double m_fMax;
    double m_fDelta;
};


#endif // DOUBLEVALIDATOR_H
