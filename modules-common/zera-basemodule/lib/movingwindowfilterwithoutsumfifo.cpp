#include "movingwindowfilterwithoutsumfifo.h"

MovingWindowFilterWithoutSumFifo::MovingWindowFilterWithoutSumFifo(float time) :
    cMovingwindowFilter(time)
{
}

void MovingWindowFilterWithoutSumFifo::addnewValues()
{
    QVector<double> newValues;
    // float -> double
    for (int i = 0; i < m_pActualValues->count(); i++)
        newValues.append(m_pActualValues->at(i));

    m_ActValueFifoList.append(newValues);
    QVector<double> sum(m_pActualValues->size(), 0.0);

    for(int fifoElement = 0; fifoElement < m_ActValueFifoList.count(); fifoElement++)
        for (int actValue = 0; actValue < m_pActualValues->count(); actValue++)
            sum[actValue] += m_ActValueFifoList[fifoElement][actValue];

    for (int i = 0; i < m_pActualValues->count(); i++)
        m_ActualValues[i] = sum[i]/m_ActValueFifoList.count();

    emit actualValues(&m_ActualValues);
}
