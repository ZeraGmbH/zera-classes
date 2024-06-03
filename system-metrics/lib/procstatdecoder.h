#ifndef PROCSTATDECODER_H
#define PROCSTATDECODER_H

#include "cputimingvalues.h"
#include <QStringList>
#include <QMap>

class ProcStatDecoder
{
public:
    // cpu idx/key: 0: average all / 1.. per CPU
    static /* cpu idx */ int decodeCpuProcLine(const QString &procCpuLineIn, CpuTimingValues &cpuTimingsOut);
    static QMap<int, CpuTimingValues> getCpuTimings();
private:
    static QStringList getCpuLines();
    static QString getProcStat();
};

#endif // PROCSTATDECODER_H
