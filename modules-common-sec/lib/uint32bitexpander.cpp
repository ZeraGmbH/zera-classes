#include "uint32bitexpander.h"

void UInt32BitExpander::reset()
{
    m_currentValueLower = 0;
    m_currentValueUpper = 0;
}

void UInt32BitExpander::setValue32(quint32 value)
{
    if (value < m_currentValueLower)
        m_currentValueUpper++;
    m_currentValueLower = value;
}

quint64 UInt32BitExpander::getExpandedValue64() const
{
    return (static_cast<quint64>(m_currentValueUpper) << 32) | m_currentValueLower;
}

double UInt32BitExpander::getExpandedValueDbl() const
{
    return static_cast<double>(getExpandedValue64());
}
