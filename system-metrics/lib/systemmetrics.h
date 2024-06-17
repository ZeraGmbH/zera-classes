#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "cpuload.h"
#include "totalmemorytracker.h"
#include <timertemplateqt.h>
#include <QObject>

class SystemMetrics : public QObject
{
    Q_OBJECT
public:
    SystemMetrics();
    CpuLoad *getCpuLoad();
    TotalMemoryTracker *getTotalMemoryTracker();
    void startCpuLoadPollTimer(int pollMs);
    void startMemoryPollTimer(int logIntervalMs);
signals:
    void sigNewValues();

private slots:
    void onCpuLoadTimer();
    void onMemoryTimer();
private:
    TimerTemplateQtPtr m_cpuLoadPollTimer;
    TimerTemplateQtPtr m_memoryPollTimer;
    CpuLoad m_cpuLoad;
    TotalMemoryTracker m_totalMemoryTracker;
};

#endif // SYSTEMMETRICS_H
