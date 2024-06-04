#ifndef CPULOAD_H
#define CPULOAD_H

#include <QMap>

class CpuLoad
{
public:
    void calcNextValues();
    QMap<QString, float> getLoadMapForDisplay() const;

private:
    static QString getCpuDisplayName(int cpuIdx);
    struct LoadRelevantData
    {
        quint64 m_totalTime = 0;
        quint64 m_loadRelevantTime = 0;
    };
    QMap<int, float> m_loadMap;
    QMap<int, LoadRelevantData> m_prevLoadRelData;
};

#endif // CPULOAD_H
