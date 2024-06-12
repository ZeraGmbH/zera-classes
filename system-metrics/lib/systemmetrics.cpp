#include "systemmetrics.h"
#include <timerfactoryqt.h>


SystemMetrics::SystemMetrics()
{
    m_cpuLoad.calcNextValues();
    m_totalMemoryTracker.CalculateMemoryUsedPercent();
}

CpuLoad *SystemMetrics::getCpuLoad()
{
    return &m_cpuLoad;
}

TotalMemoryTracker *SystemMetrics::getTotalMemoryTracker()
{
    return &m_totalMemoryTracker;
}

void SystemMetrics::onTimer()
{
    m_cpuLoad.calcNextValues();
    m_totalMemoryTracker.CalculateMemoryUsedPercent();
    emit sigNewValues();
}

void SystemMetrics::startTimer(int pollMs)
{
    m_periodicPollTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_periodicPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onTimer);
    m_periodicPollTimer->start();
}
