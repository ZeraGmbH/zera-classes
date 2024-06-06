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
        float load = totalDiff == 0.0 ? 0.0 : loadRelevantDiff / totalDiff;
        if(timingNum == 0)
            checkLimitAndSpawnWarning(load);
        m_loadMap[timingNum] = load;
        m_prevLoadRelData[timingNum] = currLoadRelData;
    }
}

bool CpuLoad::setWarningLimit(float limit)
{
    if(limit >= 0 && limit < 1) {
        m_loadWarnLimit = limit;
        return true;
    }
    return false;
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

void CpuLoad::checkLimitAndSpawnWarning(float allLoad)
{
    if(m_loadWarnLimit == 0.0) {
        m_loadWarnLimitActive = false;
        return;
    }
    if(allLoad > m_loadMax)
        m_loadMax = allLoad;
    float loadDisplayed = allLoad * 100.0;
    float limitDisplayed = m_loadWarnLimit * 100.0;
    float maxLoadDisplayed = m_loadMax * 100.0;
    if(!m_loadWarnLimitActive) {
        if(allLoad > m_loadWarnLimit) {
            qWarning("CPU load %.1f%% exceeds limit %.1f%%", loadDisplayed, limitDisplayed);
            m_loadWarnLimitActive = true;
        }
    }
    else {
        if(allLoad < m_loadWarnLimit) {
            qInfo("CPU load %.1f%% is within %.1f%% limit again - max was %.1f%%", loadDisplayed, limitDisplayed, maxLoadDisplayed);
            m_loadWarnLimitActive = false;
            m_loadMax = 0.0;
        }
    }
}
