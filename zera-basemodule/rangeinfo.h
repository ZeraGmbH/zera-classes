#ifndef RANGEINFO_H
#define RANGEINFO_H

#include <QObject>

enum MMode
{
    modeAC = 1,
    modeHF = 2,
    modeDC = 4,
    modeADJ = 8
};


enum SensorType
{
    Direct = 0x100,
    Clamp = 0x200
};


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
