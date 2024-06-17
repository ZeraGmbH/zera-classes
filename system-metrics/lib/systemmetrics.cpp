#include "systemmetrics.h"
#include <timerfactoryqt.h>


SystemMetrics::SystemMetrics()
{
    m_cpuLoad.calcNextValues();
    m_totalMemoryTracker.calculateMemoryUsedPercent();
}

CpuLoad *SystemMetrics::getCpuLoad()
{
    return &m_cpuLoad;
}

TotalMemoryTracker *SystemMetrics::getTotalMemoryTracker()
{
    return &m_totalMemoryTracker;
}

void SystemMetrics::onCpuLoadTimer()
{
    m_cpuLoad.calcNextValues();
    m_totalMemoryTracker.calculateMemoryUsedPercent();
    emit sigNewValues();
}

void SystemMetrics::onMemoryTimer()
{
    m_totalMemoryTracker.periodicLogs();
}

void SystemMetrics::startCpuLoadPollTimer(int pollMs)
{
    m_cpuLoadPollTimer = TimerFactoryQt::createPeriodic(pollMs);
    connect(m_cpuLoadPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onCpuLoadTimer);
    m_cpuLoadPollTimer->start();
}

void SystemMetrics::startMemoryPollTimer(int logIntervalMs)
{
    m_memoryPollTimer = TimerFactoryQt::createPeriodic(logIntervalMs);
    connect(m_memoryPollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SystemMetrics::onMemoryTimer);
    m_memoryPollTimer->start();
}
