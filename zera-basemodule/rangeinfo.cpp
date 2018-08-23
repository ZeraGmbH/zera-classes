#include "rangeinfo.h"

cRangeInfo::cRangeInfo(const cRangeInfo &obj)
{
    name=obj.name;
    type=obj.type;
    avail=obj.avail;
    alias=obj.alias;
    urvalue=obj.urvalue;
    rejection=obj.rejection;
    ovrejection=obj.ovrejection;
    adcrejection=obj.adcrejection;
}

cRangeInfo cRangeInfo::operator=(const cRangeInfo& obj)
{
    name=obj.name;
    type=obj.type;
    avail=obj.avail;
    alias=obj.alias;
    urvalue=obj.urvalue;
    rejection=obj.rejection;
    ovrejection=obj.ovrejection;
    adcrejection=obj.adcrejection;
    return *this;
}
