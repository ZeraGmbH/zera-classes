#include "systemmetrics.h"
#include <timerfactoryqt.h>


SystemMetrics::SystemMetrics()
{
    m_cpuLoad.calcNextValues();
}

CpuLoad *SystemMetrics::getCpuLoad()
{
    return &m_cpuLoad;
}

void SystemMetrics::onTimer()
{
    m_cpuLoad.calcNextValues();
    emit sigNewValues();
}

void SystemMetrics::startTimer(int pollMs)
{
    m_periodicPollTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_periodicPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onTimer);
    m_periodicPollTimer->start();
}
