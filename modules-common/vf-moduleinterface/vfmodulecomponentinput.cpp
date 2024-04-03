#include "vfmodulecomponentinput.h"

VfModuleComponentInput::VfModuleComponentInput(int entityid, QString name)  :
    m_nEntityId(entityid),
    m_sName(name)
{
}

QVariant VfModuleComponentInput::value() const
{
    return m_value;
}

void VfModuleComponentInput::setValue(const QVariant &value)
{
    m_value = value;
}
