#include "actualvaluegeneratorstartstop.h"

void ActualValueGeneratorStartStop::onNewActualValues(QVector<float> *values)
{
    if(m_started)
        emit sigNewActualValues(values);
}

void ActualValueGeneratorStartStop::start()
{
    m_started = true;
}

void ActualValueGeneratorStartStop::stop()
{
    m_started = false;
}
