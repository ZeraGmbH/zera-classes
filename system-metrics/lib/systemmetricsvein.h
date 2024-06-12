#ifndef SYSTEMMETRICSVEIN_H
#define SYSTEMMETRICSVEIN_H

#include "vf-cpp-entity.h"
#include "systemmetrics.h"
#include <QObject>

class SystemMetricsVein : public QObject
{
    Q_OBJECT
public:
    explicit SystemMetricsVein(int entityId, QObject *parent = nullptr);
    VeinEvent::EventSystem* getEventSystem();
    void initOnce();
private slots:
    void onPollTimeChanged(QVariant newValue);
    void onCpuWarnLimitChanged(QVariant newValue);
    void onNewValues();
private:
    VfCpp::VfCppEntity m_entity;
    int m_pollTimeMs = 1000;
    float m_cpuLoadWarnLimit = 20;
    VfCpp::VfCppComponent::Ptr m_pollTimeComponent;
    VfCpp::VfCppComponent::Ptr m_cpuLoadWarnLimitComponent;
    QMap<QString, VfCpp::VfCppComponent::Ptr> m_cpuLoadComponents;
    VfCpp::VfCppComponent::Ptr m_memoryUsedComponent;
    SystemMetrics m_systemMetrics;
};

#endif // SYSTEMMETRICSVEIN_H
