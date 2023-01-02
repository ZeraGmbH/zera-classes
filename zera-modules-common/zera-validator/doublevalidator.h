#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "validatorinterface.h"

class cDoubleValidator: public ValidatorInterface
{
public:
    cDoubleValidator() { }
    cDoubleValidator(double min, double max, double delta);
    cDoubleValidator(const cDoubleValidator& ref);
    virtual bool isValidParam(QVariant& newValue) override;
    virtual void exportMetaData(QJsonObject& jsObj) override;
private:
    double m_fMin;
    double m_fMax;
    double m_fDelta;
};


#endif // DOUBLEVALIDATOR_H
