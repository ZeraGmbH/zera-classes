#include "uint32bitexpander.h"

void UInt32BitExpander::reset()
{
    m_actualLower = 0;
    m_actualUpper = 0;

    m_finalLower = 0;
    m_finalUpper = 0;
    m_signedOffsetFinalToActual = 0;
}

void UInt32BitExpander::setActual32(quint32 value)
{
    if (value < m_actualLower || (value < m_finalLower))
        m_actualUpper++;
    m_actualLower = value;
    setFinal32(m_finalLower);
}

quint64 UInt32BitExpander::getActualExpanded64() const
{
    quint32 upper = m_actualUpper;
    /*if (upper < m_finalUpper)
        upper = m_finalUpper;*/
    return  (static_cast<quint64>(upper) << 32) | m_actualLower;
}

double UInt32BitExpander::getActualExpandedDbl() const
{
    return static_cast<double>(getActualExpanded64());
}

void UInt32BitExpander::setFinal32(quint32 value)
{
    if (value < m_finalLower)
        m_finalUpper++;
    m_finalLower = value;
    qint32 signedFinal = static_cast<qint32>(value);
    qint32 signedActual = static_cast<qint32>(m_actualLower);
    m_signedOffsetFinalToActual = signedFinal-signedActual;
}

quint64 UInt32BitExpander::getFinalExpanded64() const
{
    quint64 actual64 = getActualExpanded64();
    quint64 final64 = actual64 + m_signedOffsetFinalToActual;
    if (final64 & (1ULL << 63)) // in our lifetime bit 63 can only be set by negative underrun
        final64 += (1ULL << 32);
    return final64;
}

double UInt32BitExpander::getFinalExpandedDbl() const
{
    return static_cast<double>(getFinalExpanded64());
}
