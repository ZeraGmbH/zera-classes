#ifndef RANGEINFO_H
#define RANGEINFO_H

#include <QObject>

struct cRangeInfo
{
    QString name;
    qint32 type;
    bool avail;
    QString alias;
    double urvalue;
    double rejection;
    double ovrejection;
};

#endif // RANGEINFO_H
