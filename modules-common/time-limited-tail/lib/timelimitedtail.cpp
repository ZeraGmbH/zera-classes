#include "timelimitedtail.h"

TimeLimitedTail::TimeLimitedTail(int tailLengthMs)
{

}

void TimeLimitedTail::setTailLength(int tailLengthMs)
{

}

void TimeLimitedTail::appendValues(const QVector<float> &values)
{
    m_values.append(values);
    emit sigContentsChanged();
}

const QList<QVector<float> > &TimeLimitedTail::getValues() const
{
    return m_values;
}
