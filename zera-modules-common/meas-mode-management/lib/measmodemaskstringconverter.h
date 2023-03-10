#ifndef MEASMODEMASKSTRINGCONVERTER_H
#define MEASMODEMASKSTRINGCONVERTER_H

#include "measmodephasesetstrategy.h"
#include <QString>

class MeasModeMaskStringConverter
{
public:
    static bool calcBinMask(QString strMask, int measSysCount, MModePhaseMask &binMask);
    static QString calcStringMask(MModePhaseMask binMask, int measSysCount);
};

#endif // MEASMODEMASKSTRINGCONVERTER_H
