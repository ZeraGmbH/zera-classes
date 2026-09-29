#ifndef UINT32BITEXPANDER_H
#define UINT32BITEXPANDER_H

#include <QtGlobal>

class UInt32BitExpander
{
public:
    void reset();

    void setActual32(quint32 value);
    quint64 getActualExpanded64() const;
    double getActualExpandedDbl() const;

private:
    quint32 m_actualLower = 0;
    quint32 m_actualUpper = 0;
};

#endif // UINT32BITEXPANDER_H
