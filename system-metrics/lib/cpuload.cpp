#include "cpuload.h"
#include "procstatdecoder.h"
#include <cmath>

void CpuLoad::calcNextValues()
{
    const QMap<int, CpuTimingValues> allTimings = ProcStatDecoder::getCpuTimings();
    for(auto iter=allTimings.cbegin(); iter!=allTimings.cend(); iter++) {
        const int timingNum = iter.key();
        const CpuTimingValues& timings = iter.value();
        // inspired by https://gitlab.xfce.org/panel-plugins/xfce4-cpugraph-plugin/-/blob/master/panel-plugin/os.cc
        // Simplified: just Linux & no CPU hotplug
        LoadRelevantData currLoadRelData;
        currLoadRelData.m_loadRelevantTime =
            timings.m_systemTime +
            timings.m_irqTime +
            timings.m_softIrqTime +
            timings.m_userTime +
            timings.m_niceTime;
        currLoadRelData.m_totalTime =
            currLoadRelData.m_loadRelevantTime +
            timings.m_idleTime + timings.m_iowaitTime;

        float totalDiff = (float)(currLoadRelData.m_totalTime - m_prevLoadRelData[timingNum].m_totalTime);
        float loadRelevantDiff = (float)(currLoadRelData.m_loadRelevantTime - m_prevLoadRelData[timingNum].m_loadRelevantTime);
        float load = loadRelevantDiff / totalDiff;
        m_loadMap[timingNum] = load;
        m_prevLoadRelData[timingNum] = currLoadRelData;
    }
}

QMap<QString, float> CpuLoad::getLoadMapForDisplay() const
{
    QMap<QString, float> loadMapForDisplay;
    int nameDigits;
    if(m_loadMap.count() > 100)
        nameDigits = 3;
    else if(m_loadMap.count() > 10)
        nameDigits = 2;
    else
        nameDigits = 1;
    for(auto iter=m_loadMap.cbegin(); iter!=m_loadMap.cend(); ++iter) {
        QString cpuDisplay = getCpuDisplayName(iter.key());
        if(iter.key() != 0)
            cpuDisplay = QString("000000" + getCpuDisplayName(iter.key())).right(nameDigits);
        float displayedValue = iter.value() * 1000;
        displayedValue = std::round(displayedValue);
        displayedValue /= 10.0;
        loadMapForDisplay[cpuDisplay] = displayedValue;
    }
    return loadMapForDisplay;
}

QString CpuLoad::getCpuDisplayName(int cpuIdx)
{
    if(cpuIdx == 0)
        return "All";
    return QString("%1").arg(cpuIdx); // 1 based
}
