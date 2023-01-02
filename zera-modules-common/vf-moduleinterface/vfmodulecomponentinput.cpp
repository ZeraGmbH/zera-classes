#include "vfmodulecomponentinput.h"

QVariant VfModuleComponentInput::value() const
{
    return m_value;
}

void VfModuleComponentInput::setValue(const QVariant &value)
{
    m_value = value;
}
