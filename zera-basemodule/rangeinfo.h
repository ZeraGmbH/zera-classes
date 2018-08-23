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


class cRangeInfo
{
public:
    cRangeInfo(){}
    cRangeInfo(const cRangeInfo &obj){name=obj.name;type=obj.type;avail=obj.avail;alias=obj.alias;urvalue=obj.urvalue;rejection=obj.rejection;ovrejection=obj.ovrejection;adcrejection=obj.adcrejection;}

    QString name;
    qint32 type;
    bool avail;
    QString alias;
    double urvalue;
    double rejection;
    double ovrejection;
    double adcrejection;
};

#endif // RANGEINFO_H
