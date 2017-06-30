#ifndef ADJUSTVALIDATOR
#define ADJUSTVALIDATOR

#include <QString>
#include <QStringList>
#include <QHash>

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


class cAdjustValidator3d: public cParamValidator
{
public:
    cAdjustValidator3d(){}
    virtual ~cAdjustValidator3d();

    virtual void addValidator(QString chnName, QStringList rList, cDoubleValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatad*> m_adjustValidatorHash; // the channels name is the key
};


class cAdjustValidator3i: public cParamValidator
{
public:
    cAdjustValidator3i(){}
    cAdjustValidator3i(const cAdjustValidator3i& ref);
    virtual ~cAdjustValidator3i(){}

    virtual void addValidator(QString chnName, QStringList rList, cIntValidator parVal);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, adjValidatorDatai*> m_adjustValidatorHash; // the channels name is the key
};



class cAdjustValidator2: public cParamValidator
{
public:
    cAdjustValidator2(){}
    virtual ~cAdjustValidator2(){}

    virtual void addValidator(QString chnName, QStringList rList);

    virtual bool isValidParam(QVariant& newValue);
    virtual void exportMetaData(QJsonObject& jsObj);

private:
    QHash<QString, QStringList*> m_adjustValidatorHash; // the channels name is the key
};

#endif // ADJUSTVALIDATOR

