#include <QJsonArray>
#include <QJsonObject>


#include "adjustvalidator.h"

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
    adjValidatorDatad* adjValidator;
    adjValidator->rangeList = rList;
    adjValidator->dValidator = parVal;

    m_adjustValidatorHash[chnName] = adjValidator;
}


bool cAdjustValidator3d::isValidParam(QVariant &newValue)
{
    QStringList sl;

    sl = newValue.toString().split(';');
    if ((sl.count() == 4) && (sl.at(3) == ""))
    {
        QString key;
        key = sl.at(0);
        if (m_adjustValidatorHash.contains(key))
            if (m_adjustValidatorHash[key]->rangeList.contains(sl.at(1)))
            {
                QVariant var;
                var = (QVariant)(sl.at(2).toDouble());
                if (m_adjustValidatorHash[key]->dValidator.isValidParam(var))
                    return true;
            }
    }

    return false;
}


void cAdjustValidator3d::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert("Type", "ADJUSTDATA");

    QJsonArray jsonArr = {"not subject"};
    jsObj.insert("Data", jsonArr);
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

    sl = newValue.toString().split(';');
    if ((sl.count() == 3) && (sl.at(2) == ""))
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


void cAdjustValidator3i::addValidator(QString chnName, QStringList rList, cIntValidator parVal)
{
    adjValidatorDatai* adjValidator;
    adjValidator->rangeList = rList;
    adjValidator->iValidator = parVal;

    m_adjustValidatorHash[chnName] = adjValidator;
}


bool cAdjustValidator3i::isValidParam(QVariant &newValue)
{
    QStringList sl;

    sl = newValue.toString().split(';');
    if ((sl.count() == 4) && (sl.at(3) == ""))
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
