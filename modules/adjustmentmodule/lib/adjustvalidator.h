#ifndef ADJUSTVALIDATOR
#define ADJUSTVALIDATOR

#include "adjustmentmodulemeasprogram.h"
#include "doublevalidator.h"
#include "intvalidator.h"
#include <QString>
#include <QStringList>
#include <QHash>

class adjValidatorDatad
{
public:
    QStringList rangeList;
    cDoubleValidator dValidator;
};

class adjValidatorDatai
{
public:
    QStringList rangeList;
    cIntValidator iValidator;
};

class cAdjustValidator3d: public ValidatorInterface
{
public:
    explicit cAdjustValidator3d(cAdjustmentModuleMeasProgram *measprogram);

    void addValidator(const QString &chnName, const QStringList &rList, const cDoubleValidator &parVal);
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;

private:
    QHash<QString, adjValidatorDatad> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};


class cAdjustValidator3i: public ValidatorInterface
{
public:
    explicit cAdjustValidator3i(cAdjustmentModuleMeasProgram *measprogram);
    explicit cAdjustValidator3i(const cAdjustValidator3i& ref);

    void addValidator(const QString &chnName, const QStringList &rList, const cIntValidator &parVal);
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;

private:
    QHash<QString, adjValidatorDatai> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};



class cAdjustValidator2: public ValidatorInterface
{
public:
    explicit cAdjustValidator2(cAdjustmentModuleMeasProgram *measprogram);

    void addValidator(const QString &chnName, const QStringList &rList);
    bool isValidParam(QVariant& newValue) override;
    void exportMetaData(QJsonObject& jsObj) override;

private:
    QHash<QString, QStringList> m_adjustValidatorHash; // the channels name is the key
    cAdjustmentModuleMeasProgram *m_pMeasprogram;
};


class cAdjustValidatorFine: public ValidatorInterface
{
public:
    bool isValidParam(QVariant& ) override;
    void exportMetaData(QJsonObject& jsObj) override;
};

#endif // ADJUSTVALIDATOR

