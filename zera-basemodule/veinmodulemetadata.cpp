#include <QJsonObject>

#include "veinmodulemetadata.h"


cVeinModuleMetaData::cVeinModuleMetaData(QString name, QVariant value)
    :m_sName(name), m_vValue(value)
{
}


void cVeinModuleMetaData::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert(m_sName, m_vValue.toString());
}


void cVeinModuleMetaData::setValue(QVariant value)
{
    m_vValue = value;
}
