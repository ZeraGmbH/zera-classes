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
    m_cpuLoadWarnLimitComponent= m_entity.createComponent("PAR_LoadWarnLimit", m_cpuLoadWarnLimit);
    connect(m_cpuLoadWarnLimitComponent.get(), &VfCpp::VfCppComponent::sigValueChanged,
            this, &SystemMetricsVein::onCpuWarnLimitChanged);

    CpuLoad* cpuLoad = m_systemMetrics.getCpuLoad();
    QMap<QString, float> cpuInfo = cpuLoad->getLoadMapForDisplay();
    for(auto iter=cpuInfo.cbegin(); iter!=cpuInfo.cend(); ++iter)
        m_cpuLoadComponents[iter.key()] = m_entity.createComponent(QString("ACT_CPU_%1_LOAD").arg(iter.key()), float(0.0), true);
    connect(&m_systemMetrics, &SystemMetrics::sigNewValues,
            this, &SystemMetricsVein::onNewValues);
    m_systemMetrics.startTimer(m_pollTimeMs);
}

void SystemMetricsVein::onPollTimeChanged(QVariant newValue)
{
    if(newValue.toInt() >= 500 && newValue.toInt() <= 100000) {
        m_pollTimeMs = newValue.toInt();
        m_systemMetrics.startTimer(m_pollTimeMs);
    }
    else
        m_pollTimeComponent->setValue(m_pollTimeMs);
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
}
