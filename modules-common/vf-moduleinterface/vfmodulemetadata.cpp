#include <QJsonObject>
#include "vfmodulemetadata.h"

int VfModuleMetaData::m_instanceCount = 0;

VfModuleMetaData::VfModuleMetaData(QString name, QVariant value) :
    m_sName(name),
    m_vValue(value)
{
    m_instanceCount++;
}

VfModuleMetaData::~VfModuleMetaData()
{
    m_instanceCount--;
}

void VfModuleMetaData::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert(m_sName, m_vValue.toString());
}

int VfModuleMetaData::getInstanceCount()
{
    return m_instanceCount;
}

void VfModuleMetaData::setValue(QVariant value)
{
    m_vValue = value;
}
