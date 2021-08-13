#include "veinmodulecomponentinput.h"



QVariant cVeinModuleComponentInput::value() const
{
    return m_value;
}

void cVeinModuleComponentInput::setValue(const QVariant &value)
{
    m_value = value;
}
