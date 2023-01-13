#ifndef ADJUSTVALIDATOR
#define ADJUSTVALIDATOR

#include <QString>
#include <QStringList>
#include <QHash>

#include "adjustmentmodulemeasprogram.h"
#include "doublevalidator.h"
#include "intvalidator.h"

class adjValidatorDatad
{
public:
    adjValidatorDatad(){}
    QStringList rangeList;
    cDoubleValidator dValidator;
};


class adjValidatorDatai
{
public:
    adjValidatorDatai(){}
    QStringList rangeList;
    cIntValidator iValidator;
};


class cAdjustValidator3d: public ValidatorInterface
{
public:
    cAdjustValidator3d(cAdjustmentModuleMeasProgram *measprogram);
    virtual ~cAdjustValidator3d();

    virtual void addValidator(QString chnName, QStringList rList, cDoubleValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatad*> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};


class cAdjustValidator3i: public ValidatorInterface
{
public:
    cAdjustValidator3i(cAdjustmentModuleMeasProgram *measprogram);
    cAdjustValidator3i(const cAdjustValidator3i& ref);
    virtual ~cAdjustValidator3i(){}

    virtual void addValidator(QString chnName, QStringList rList, cIntValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatai*> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};



class cAdjustValidator2: public ValidatorInterface
{
public:
    cAdjustValidator2(cAdjustmentModuleMeasProgram *measprogram);
    virtual ~cAdjustValidator2(){}

    virtual void addValidator(QString chnName, QStringList rList);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, QStringList*> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};


class cAdjustValidatorFine: public ValidatorInterface
{
public:
    cAdjustValidatorFine(){}
    virtual ~cAdjustValidatorFine(){}

    virtual bool isValidParam(QVariant& );
    virtual void exportMetaData(QJsonObject& jsObj);
};

#endif // ADJUSTVALIDATOR

