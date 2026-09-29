#include "uint32bitexpander.h"

void UInt32BitExpander::reset()
{
    m_actualLower = 0;
    m_actualUpper = 0;
}

void UInt32BitExpander::setActual32(quint32 value)
{
    if (value < m_actualLower)
        m_actualUpper++;
    m_actualLower = value;
}

quint64 UInt32BitExpander::getActualExpanded64() const
{
    return (static_cast<quint64>(m_actualUpper) << 32) | m_actualLower;
}

double UInt32BitExpander::getActualExpandedDbl() const
{
    return static_cast<double>(getActualExpanded64());
}
