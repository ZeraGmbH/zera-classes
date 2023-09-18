#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include "validatorinterface.h"

class cDoubleValidator: public ValidatorInterface
{
public:
    cDoubleValidator() { }
    cDoubleValidator(double min, double max, double delta);
    cDoubleValidator(const cDoubleValidator& ref);
    double getMinValue();
    double getMaxValue();
    double getDelta();
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
private:
    double m_fMin;
    double m_fMax;
    double m_fDelta;
};


#endif // DOUBLEVALIDATOR_H
