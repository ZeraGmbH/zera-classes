#include "measmodemaskstringconverter.h"

bool MeasModeMaskStringConverter::calcBinMask(QString mask, int measSysCount, MModePhaseMask &binMask)
{
    bool ok = mask.size() == measSysCount;
    if(ok) {
        MModePhaseMask tmpMask;
        for(int i=0; i<measSysCount; i++) {
            int strMaskEntry = measSysCount-i-1;
            if(mask[strMaskEntry] == '0')
                tmpMask[i] = 0;
            else if(mask[strMaskEntry] == '1')
                tmpMask[i] = 1;
            else {
                ok = false;
                break;
            }
        }
        if(ok)
            binMask = tmpMask;
    }
    return ok;
}

QString MeasModeMaskStringConverter::calcStringMask(MModePhaseMask binMask, int measSysCount)
{
    QString mask;
    for(int i=std::min(MeasPhaseCount, measSysCount)-1; i>=0; i--)
        mask += binMask[i] ? "1" : "0";
    return mask;
}

