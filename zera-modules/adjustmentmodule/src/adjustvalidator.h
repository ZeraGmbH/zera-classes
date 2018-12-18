#ifndef ADJUSTVALIDATOR
#define ADJUSTVALIDATOR

#include <QString>
#include <QStringList>
#include <QHash>

#include "adjustmentmodulemeasprogram.h"
#include "doublevalidator.h"
#include "intvalidator.h"

class cAdjustmentModuleMeasProgram;

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


class cAdjustValidator3d: public cParamValidator
{
public:
    cAdjustValidator3d(cAdjustmentModuleMeasProgram *measprogram);
    virtual ~cAdjustValidator3d();

    virtual void addValidator(QString chnName, QStringList rList, cDoubleValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatad*> m_adjustValidatorHash; // the channels name is the key
<<<<<<< Updated upstream
    ADJUSTMENTMODULE::cAdjustmentModuleMeasProgram *m_pMeasprogram;
=======
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
>>>>>>> Stashed changes
};


class cAdjustValidator3i: public cParamValidator
{
public:
<<<<<<< Updated upstream
    cAdjustValidator3i(ADJUSTMENTMODULE::cAdjustmentModuleMeasProgram *measprogram);
=======
    cAdjustValidator3i(cAdjustmentModuleMeasProgram *measprogram);
>>>>>>> Stashed changes
    cAdjustValidator3i(const cAdjustValidator3i& ref);
    virtual ~cAdjustValidator3i(){}

    virtual void addValidator(QString chnName, QStringList rList, cIntValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatai*> m_adjustValidatorHash; // the channels name is the key
<<<<<<< Updated upstream
    ADJUSTMENTMODULE::cAdjustmentModuleMeasProgram *m_pMeasprogram;
=======
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
>>>>>>> Stashed changes
};



class cAdjustValidator2: public cParamValidator
{
public:
<<<<<<< Updated upstream
    cAdjustValidator2(ADJUSTMENTMODULE::cAdjustmentModuleMeasProgram *measprogram);
=======
    cAdjustValidator2(cAdjustmentModuleMeasProgram *measprogram);
>>>>>>> Stashed changes
    virtual ~cAdjustValidator2(){}

    virtual void addValidator(QString chnName, QStringList rList);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, QStringList*> m_adjustValidatorHash; // the channels name is the key
<<<<<<< Updated upstream
    ADJUSTMENTMODULE::cAdjustmentModuleMeasProgram *m_pMeasprogram;
=======
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
>>>>>>> Stashed changes
};

#endif // ADJUSTVALIDATOR

