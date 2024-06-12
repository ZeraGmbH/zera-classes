#ifndef TOTALMEMORYTRACKER_H
#define TOTALMEMORYTRACKER_H

#include <QObject>

class TotalMemoryTracker : public QObject
{
    Q_OBJECT
public:
    float CalculateMemoryUsedPercent();
};

#endif // TOTALMEMORYTRACKER_H
