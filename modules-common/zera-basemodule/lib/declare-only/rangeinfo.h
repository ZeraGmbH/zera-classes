#ifndef RANGEINFO_H
#define RANGEINFO_H

#include <QString>

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
