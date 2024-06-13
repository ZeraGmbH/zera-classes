#ifndef TOTALMEMORYTRACKER_H
#define TOTALMEMORYTRACKER_H

#include <QMap>

struct memoryUsageParams {
    float m_RAMUsedPercent = 0.0;
    float m_buffersAndCachedUsedPercent = 0.0;
};

class TotalMemoryTracker
{
public:
    void CalculateMemoryUsedPercent();
    memoryUsageParams getMemoryUsageParams() const;
    void periodicLogs();
private:
    memoryUsageParams m_memoryUsageParams;
};

#endif // TOTALMEMORYTRACKER_H
