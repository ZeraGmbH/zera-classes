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
    void checkLimitAndSpawnWarning(float allLoad);
    struct LoadRelevantData
    {
        quint64 m_totalTime = 0;
        quint64 m_loadRelevantTime = 0;
    };
    QMap<int, float> m_loadMap;
    QMap<int, LoadRelevantData> m_prevLoadRelData;

    float m_loadWarnLimit = 0.0;
    float m_loadMax = 0.0;
    bool m_loadWarnLimitActive = false;
};

#endif // CPULOAD_H
