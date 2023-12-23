#ifndef RANGEINFO_H
#define RANGEINFO_H

#include <QString>

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

class cRangeInfoBase
{
public:
    QString name;
    qint32 type;
    bool avail;
    QString alias;
};

class cRangeInfoWithConstantValues : public cRangeInfoBase
{
public:
    double urvalue;
    double rejection;
    double ovrejection;
    double adcrejection;
};


#endif // RANGEINFO_H
