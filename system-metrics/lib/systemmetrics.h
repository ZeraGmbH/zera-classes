#ifndef SYSTEMMETRICS_H
#define SYSTEMMETRICS_H

#include "cpuload.h"
#include <timertemplateqt.h>
#include <QObject>

class SystemMetrics : public QObject
{
    Q_OBJECT
public:
    SystemMetrics();
    CpuLoad *getCpuLoad();
    void startTimer(int pollMs);
signals:
    void sigNewValues();

private slots:
    void onTimer();
private:
    TimerTemplateQtPtr m_periodicPollTimer;
    CpuLoad m_cpuLoad;
};

#endif // SYSTEMMETRICS_H
