#include "systemmetricsvein.h"

SystemMetricsVein::SystemMetricsVein(int entityId, QObject *parent) :
    QObject{parent},
    m_entity(entityId)
{
}

VeinEvent::EventSystem *SystemMetricsVein::getEventSystem()
{
    return &m_entity;
}

void SystemMetricsVein::initOnce()
{
    m_entity.initModule();
    m_entity.createComponent("EntityName", "_SystemMetrics", true);

    m_pollTimeComponent = m_entity.createComponent("PAR_PollTimeMs", m_pollTimeMs);
    connect(m_pollTimeComponent.get(), &VfCpp::VfCppComponent::sigValueChanged,
            this, &SystemMetricsVein::onPollTimeChanged);
    m_logIntervalComponent = m_entity.createComponent("PAR_LogIntervalMs", m_logIntervalMs);
    connect(m_logIntervalComponent.get(), &VfCpp::VfCppComponent::sigValueChanged,
            this, &SystemMetricsVein::onLogIntervalChanged);
    m_cpuLoadWarnLimitComponent= m_entity.createComponent("PAR_LoadWarnLimit", m_cpuLoadWarnLimit);
    connect(m_cpuLoadWarnLimitComponent.get(), &VfCpp::VfCppComponent::sigValueChanged,
            this, &SystemMetricsVein::onCpuWarnLimitChanged);

    CpuLoad* cpuLoad = m_systemMetrics.getCpuLoad();
    QMap<QString, float> cpuInfo = cpuLoad->getLoadMapForDisplay();
    for(auto iter=cpuInfo.cbegin(); iter!=cpuInfo.cend(); ++iter)
        m_cpuLoadComponents[iter.key()] = m_entity.createComponent(QString("ACT_CPU_%1_LOAD").arg(iter.key()), float(0.0), true);

    TotalMemoryTracker* memoryTracker = m_systemMetrics.getTotalMemoryTracker();
    m_RAMUsedComponent = m_entity.createComponent(QString("ACT_MEM_RAM_USED"), memoryTracker->getMemoryUsageParams().m_RAMUsedPercent);
    m_buffersUsedComponent = m_entity.createComponent(QString("ACT_MEM_BUFFERS_USED"), memoryTracker->getMemoryUsageParams().m_buffersUsedPercent);
    m_cachesUsedComponent = m_entity.createComponent(QString("ACT_MEM_CASHES_USED"), memoryTracker->getMemoryUsageParams().m_cachesUsedPercent);

    connect(&m_systemMetrics, &SystemMetrics::sigNewValues,
            this, &SystemMetricsVein::onNewValues);
    m_systemMetrics.startCpuLoadPollTimer(m_pollTimeMs);
    m_systemMetrics.startMemoryPollTimer(m_logIntervalMs);
    m_systemMetrics.getCpuLoad()->setWarningLimit(((float)m_cpuLoadWarnLimit)/100.0);
}

void SystemMetricsVein::onPollTimeChanged(QVariant newValue)
{
    if(newValue.toInt() >= 500 && newValue.toInt() <= 100000) {
        m_pollTimeMs = newValue.toInt();
        m_systemMetrics.startCpuLoadPollTimer(m_pollTimeMs);
    }
    else
        m_pollTimeComponent->setValue(m_pollTimeMs);
}

void SystemMetricsVein::onLogIntervalChanged(QVariant newValue)
{
    if(newValue.toInt() >= 1000 && newValue.toInt() <= 100000) {
        m_logIntervalMs = newValue.toInt();
        m_systemMetrics.startMemoryPollTimer(m_logIntervalMs);
    }
    else
        m_logIntervalComponent->setValue(m_logIntervalMs);
}

void SystemMetricsVein::onCpuWarnLimitChanged(QVariant newValue)
{
    if(newValue.toInt() >= 0 && newValue.toInt() <= 100) { // 0: no warnings
        m_cpuLoadWarnLimit = newValue.toInt();
        m_systemMetrics.getCpuLoad()->setWarningLimit(((float)m_cpuLoadWarnLimit)/100.0);
    }
    else
        m_cpuLoadWarnLimitComponent->setValue(m_cpuLoadWarnLimit);
}

void SystemMetricsVein::onNewValues()
{
    CpuLoad* cpuLoad = m_systemMetrics.getCpuLoad();
    QMap<QString, float> cpuInfo = cpuLoad->getLoadMapForDisplay();
    for(auto iter=cpuInfo.cbegin(); iter!=cpuInfo.cend(); ++iter) {
        if(m_cpuLoadComponents.contains(iter.key()))
            m_cpuLoadComponents[iter.key()]->setValue(iter.value());
        else
            qWarning("SystemMetricsVein: unkown CPU %s", qPrintable(iter.key()));
    }

    TotalMemoryTracker* memoryTracker = m_systemMetrics.getTotalMemoryTracker();
    m_RAMUsedComponent->setValue(memoryTracker->getMemoryUsageParams().m_RAMUsedPercent);
    m_buffersUsedComponent->setValue(memoryTracker->getMemoryUsageParams().m_buffersUsedPercent);
    m_cachesUsedComponent->setValue(memoryTracker->getMemoryUsageParams().m_cachesUsedPercent);
}
