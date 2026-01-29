#include <QJsonObject>
#include "vfmodulemetadata.h"

int VfModuleMetaData::m_instanceCount = 0;

VfModuleMetaData::VfModuleMetaData(const QString &moduleMetaEntryName, const QVariant &value) :
    m_moduleMetaEntryName(moduleMetaEntryName),
    m_value(value)
{
    m_instanceCount++;
}

VfModuleMetaData::~VfModuleMetaData()
{
    m_instanceCount--;
}

void VfModuleMetaData::exportMetaData(QJsonObject &jsObj)
{
    jsObj.insert(m_moduleMetaEntryName, m_value.toString());
}

int VfModuleMetaData::getInstanceCount()
{
    return m_instanceCount;
}

void VfModuleMetaData::setValue(QVariant value)
{
    m_value = value;
}
