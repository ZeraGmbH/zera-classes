#ifndef TOTALMEMORYTRACKER_H
#define TOTALMEMORYTRACKER_H

#include <QMap>

struct MemoryUsageParams {
    float m_RAMUsedPercent = 0.0;
    float m_cachesUsedPercent = 0.0;
    float m_buffersUsedPercent = 0.0;
    void setZeros();
};

class TotalMemoryTracker
{
public:
    void calculateMemoryUsedPercent();
    MemoryUsageParams getMemoryUsageParams() const;
    void periodicLogs();
private:
    float calcPercentageOneDecimal(float value);
    MemoryUsageParams m_memoryUsageParams;
    QString m_lastLogString;
};

#endif // TOTALMEMORYTRACKER_H
