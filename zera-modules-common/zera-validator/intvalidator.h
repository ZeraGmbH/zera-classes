#ifndef INTVALIDATOR_H
#define INTVALIDATOR_H

#include "validatorinterface.h"

class cIntValidator: public ValidatorInterface
{
public:
    cIntValidator() { }
    cIntValidator(qint64 min, qint64 max, qint64 delta = 0);
    cIntValidator(const cIntValidator& ref);
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;
private:
    qint64 m_nMin;
    qint64 m_nMax;
    qint64 m_nDelta;
};

#endif // INTVALIDATOR_H
