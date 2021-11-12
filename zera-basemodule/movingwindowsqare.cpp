#include <math.h>
#include "movingwindowsqare.h"

cMovingwindowSqare::cMovingwindowSqare(float time)
    :cMovingwindowFilter(time)
{
}

void cMovingwindowSqare::addnewValues()
{
    int n;
    QVector<double> newValues;

    n = m_pActualValues->count();
    for (int i = 0; i < n; i++) {
        float val;
        val = m_pActualValues->at(i);
        newValues.append(val * val);
    }

    m_ActValueFifoList.append(newValues);
    int m = m_ActValueFifoList.count();
    for (int i = 0; i < n; i++) {
        m_FifoSum.replace(i, m_FifoSum.at(i) + newValues.at(i));
        m_ActualValues.replace(i, sqrt(m_FifoSum.at(i) / m)); // our filtered actual values
    }
    emit actualValues(&m_ActualValues);
}
