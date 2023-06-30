#include <QJsonObject>
#include "vfmodulemetadata.h"

VfModuleMetaData::VfModuleMetaData(QString name, QVariant value)
    :m_sName(name), m_vValue(value)
{
}

void VfModuleMetaData::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert(m_sName, m_vValue.toString());
}

void VfModuleMetaData::setValue(QVariant value)
{
    m_vValue = value;
}
