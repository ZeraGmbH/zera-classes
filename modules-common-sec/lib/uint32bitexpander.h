#ifndef UINT32BITEXPANDER_H
#define UINT32BITEXPANDER_H

#include <QtGlobal>

class UInt32BitExpander
{
public:
    void reset();
    void setValue32(quint32 value);
    quint64 getExpandedValue64() const;
    double getExpandedValueDbl() const;
private:
    quint32 m_currentValueLower = 0;
    quint32 m_currentValueUpper = 0;
};

#endif // UINT32BITEXPANDER_H
