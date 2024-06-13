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
signals:
    void sigNewValues();

private slots:
    void onPollTimer();
private:
    TimerTemplateQtPtr m_periodicPollTimer;
    CpuLoad m_cpuLoad;
    TotalMemoryTracker m_totalMemoryTracker;
};

#endif // SYSTEMMETRICS_H
