#ifndef UINT32BITEXPANDER_H
#define UINT32BITEXPANDER_H

#include <QtGlobal>

// Assumptions - see tests
// * actual is countining upwards mononotonic
// * final is close to actual
class UInt32BitExpander
{
public:
    void reset();

    void setActual32(quint32 value);
    quint64 getActualExpanded64() const;

    void setFinal32(quint32 value);
    quint64 getFinalExpanded64() const;

    static double uint64ToDbl(const quint64 &value);

private:
    void adjustFinalOffsetToActual();

    quint32 m_actualLower = 0;
    quint32 m_actualUpper = 0;

    quint32 m_finalLower = 0;
    qint32 m_signedOffsetFinalToActual = 0;
};

#endif // UINT32BITEXPANDER_H
