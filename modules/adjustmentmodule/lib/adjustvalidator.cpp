#include <QJsonArray>
#include <QJsonObject>

#include "adjustmentmodulemeasprogram.h"
#include "adjustvalidator.h"

cAdjustValidator3d::cAdjustValidator3d(cAdjustmentModuleMeasProgram *measprogram)
    :m_pMeasprogram(measprogram)
{
}

cAdjustValidator3d::~cAdjustValidator3d()
{
    QStringList keylist;

    keylist = m_adjustValidatorHash.keys();
    for (int i = 0; i < keylist.count(); i++)
    {
        adjValidatorDatad* adjValidator;
        adjValidator = m_adjustValidatorHash[keylist.at(i)];
        delete adjValidator;
    }
}


void cAdjustValidator3d::addValidator(QString chnName, QStringList rList, cDoubleValidator parVal)
{
    adjValidatorDatad* adjValidator = new adjValidatorDatad();
    adjValidator->rangeList = rList;
    adjValidator->dValidator = parVal;

    m_adjustValidatorHash[chnName] = adjValidator;
}


bool cAdjustValidator3d::isValidParam(QVariant &newValue)
{
    QStringList sl = newValue.toString().split(',');
    if (sl.count() == 3) {
        QString key = sl.at(0);
        if (m_adjustValidatorHash.contains(key))
            if (m_adjustValidatorHash[key]->rangeList.contains(sl.at(1))) {
                QVariant var;
                var = (QVariant)(sl.at(2).toDouble());
                if (m_adjustValidatorHash[key]->dValidator.isValidParam(var))
                    return true;
            }
    }
    qWarning("cAdjustValidator3d: Param %s is invalid!", qPrintable(newValue.toString()));
    return false;
}


void cAdjustValidator3d::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "ADJUSTDATA");

    QJsonArray jsonArr = {"not subject"};
    jsObj.insert("Data", jsonArr);
}


cAdjustValidator2::cAdjustValidator2(cAdjustmentModuleMeasProgram *measprogram)
    :m_pMeasprogram(measprogram)
{
}


void cAdjustValidator2::addValidator(QString chnName, QStringList rList)
{
    QStringList* sl;
    sl = new QStringList(rList);

    m_adjustValidatorHash[chnName] = sl;
}


bool cAdjustValidator2::isValidParam(QVariant &newValue)
{
    QStringList sl;

    sl = newValue.toString().split(',');
    if (sl.count() == 2)
    {
        QString key;
        key = sl.at(0);
        if (m_adjustValidatorHash.contains(key))
            if (m_adjustValidatorHash[key]->contains(sl.at(1)))
                return true;
    }

    return false;
}


void cAdjustValidator2::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "ADJUSTDATA");

    QJsonArray jsonArr = {"not subject"};
    jsObj.insert("Data", jsonArr);
}


cAdjustValidator3i::cAdjustValidator3i(cAdjustmentModuleMeasProgram *measprogram)
    :m_pMeasprogram(measprogram)
{
}


cAdjustValidator3i::cAdjustValidator3i(const cAdjustValidator3i &ref)
{
    m_adjustValidatorHash = ref.m_adjustValidatorHash;
    m_pMeasprogram = ref.m_pMeasprogram;
}


void cAdjustValidator3i::addValidator(QString chnName, QStringList rList, cIntValidator parVal)
{
    adjValidatorDatai* adjValidator = new adjValidatorDatai();
    adjValidator->rangeList = rList;
    adjValidator->iValidator = parVal;

    m_adjustValidatorHash[chnName] = adjValidator;
}


bool cAdjustValidator3i::isValidParam(QVariant &newValue)
{
    QStringList sl;

    sl = newValue.toString().split(',');
    if (sl.count() == 3)
    {
        QString key;
        key = sl.at(0);
        if (m_adjustValidatorHash.contains(key))
            if (m_adjustValidatorHash[key]->rangeList.contains(sl.at(1)))
            {
                QVariant var = (QVariant)(sl.at(2).toInt());
                if (m_adjustValidatorHash[key]->iValidator.isValidParam(var))
                    return true;
            }
    }

    return false;
}


void cAdjustValidator3i::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "ADJUSTDATA");

    QJsonArray jsonArr = {"not subject"};
    jsObj.insert("Data", jsonArr);
}


bool cAdjustValidatorFine::isValidParam(QVariant &)
{
    return true; // always good
}


void cAdjustValidatorFine::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "ADJUSTDATA");

    QJsonArray jsonArr = {"not subject"};
    jsObj.insert("Data", jsonArr);
}
