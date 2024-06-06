#ifndef CPULOAD_H
#define CPULOAD_H

#include <QMap>

class CpuLoad
{
public:
    void calcNextValues();
    bool setWarningLimit(float limit);
    QMap<QString, float> getLoadMapForDisplay() const;

private:
    static QString getCpuDisplayName(int cpuIdx);
    void checkLimitAndSpawnWarning(float allCpuLoad);
    struct LoadRelevantData
    {
        quint64 m_totalTime = 0;
        quint64 m_loadRelevantTime = 0;
    };
    QMap<int, float> m_loadMap;
    QMap<int, LoadRelevantData> m_prevLoadRelData;

    float m_allCpuLoadWarnLimit = 0.0;
    float m_allCpuLoadMax = 0.0;
    bool m_allCpuloadAboveWarnLimit = false;
};

#endif // CPULOAD_H
