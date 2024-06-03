#include "procstatdecoder.h"
#include "systeminfofilelocator.h"
#include <QFile>

int ProcStatDecoder::decodeCpuProcLine(const QString &procCpuLineIn, CpuTimingValues &cpuTimingsOut)
{
    QStringList entries = procCpuLineIn.split(" ", Qt::SkipEmptyParts);
    int cpuNum = 0;
    if(entries[0] != "cpu")
        cpuNum = entries[0].replace("cpu", "").toULongLong()+1;
    cpuTimingsOut.m_userTime = entries[1].toULongLong();
    cpuTimingsOut.m_niceTime = entries[2].toULongLong();
    cpuTimingsOut.m_systemTime = entries[3].toULongLong();
    cpuTimingsOut.m_idleTime = entries[4].toULongLong();
    cpuTimingsOut.m_iowaitTime = entries[5].toULongLong();
    cpuTimingsOut.m_irqTime = entries[6].toULongLong();
    cpuTimingsOut.m_softIrqTime = entries[7].toULongLong();
    return cpuNum;
}

QMap<int, CpuTimingValues> ProcStatDecoder::getCpuTimings()
{
    const QStringList &procCpuLines = getCpuLines();
    QMap<int, CpuTimingValues> allTimings;
    for(const QString &line : procCpuLines) {
        CpuTimingValues cpuTimings;
        int cpuNum = decodeCpuProcLine(line, cpuTimings);
        allTimings[cpuNum] = cpuTimings;
    }
    return allTimings;
}

QStringList ProcStatDecoder::getCpuLines()
{
    const QStringList procLines = getProcStat().split("\n", Qt::SkipEmptyParts);
    QStringList cpuLines;
    for(const QString &line : procLines)
        if(line.startsWith("cpu"))
            cpuLines.append(line);
    return cpuLines;
}


QString ProcStatDecoder::getProcStat()
{
    QFile file(SystemInfoFileLocator::getProcStatusFileName());
    QString procStat;
    if(file.open(QFile::ReadOnly))
        procStat = file.readAll();
    return procStat;
}
