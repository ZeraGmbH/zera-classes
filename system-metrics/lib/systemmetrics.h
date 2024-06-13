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
    void startPollTimer(int pollMs);
    void startLogTimer(int logIntervalMs);
signals:
    void sigNewValues();

private slots:
    void onPollTimer();
    void onLogTimer();
private:
    TimerTemplateQtPtr m_periodicPollTimer;
    TimerTemplateQtPtr m_periodicLogTimer;
    CpuLoad m_cpuLoad;
    TotalMemoryTracker m_totalMemoryTracker;
};

#endif // SYSTEMMETRICS_H
