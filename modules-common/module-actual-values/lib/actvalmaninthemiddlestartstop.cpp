#include "actvalmaninthemiddlestartstop.h"

void ActValManInTheMiddleStartStop::onNewActualValues(QVector<float> *values)
{
    if(m_started)
        emit sigNewActualValues(values);
}

void ActValManInTheMiddleStartStop::start()
{
    m_started = true;
}

void ActValManInTheMiddleStartStop::stop()
{
    m_started = false;
}
