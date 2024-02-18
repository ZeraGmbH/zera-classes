#include "actualvaluestartstophandler.h"

void ActualValueStartStopHandler::start()
{
    m_started = true;
}

void ActualValueStartStopHandler::stop()
{
    m_started = false;
}

void ActualValueStartStopHandler::onNewActualValues(QVector<float> *values)
{
    if(m_started)
        emit sigNewActualValues(values);
}
