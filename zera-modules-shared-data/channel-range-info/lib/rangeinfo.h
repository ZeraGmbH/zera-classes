#ifndef RANGEINFO_H
#define RANGEINFO_H

#include <QString>

class RangeInfo
{
public:
    RangeInfo();
    QString getName() const;
    QString getAlias() const;
    double getUrValue() const;
    double getRejection() const;
    double getOvrRejection() const;
    bool supportsDC() const;
};

#endif // RANGEINFO_H
