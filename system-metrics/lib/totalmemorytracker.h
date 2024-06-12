#ifndef TOTALMEMORYTRACKER_H
#define TOTALMEMORYTRACKER_H

#include <QMap>

class TotalMemoryTracker
{
public:
    void CalculateMemoryUsedPercent();
    float getMemoryUsedPercent() const;
private:
    float m_memoryUsedPercent = 0.0;
};

#endif // TOTALMEMORYTRACKER_H
