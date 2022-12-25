#ifndef TIMERRUNNERFORTEST_H
#define TIMERRUNNERFORTEST_H

#include <QMap>
#include <QList>

class TimerForTestInterface
{
public:
    virtual void fireExpired() = 0;
};

class TimerRunnerForTest
{
public:
    static TimerRunnerForTest *getInstance();
    void addTimer(TimerForTestInterface* timer, int expiredMs, bool periodic);
    void removeTimer(TimerForTestInterface* timer);
    void processTimers(int durationMs);
    int getCurrentTimeMs();
private:
    TimerRunnerForTest();
    int calcExpireTime(int expiredMs);
    static TimerRunnerForTest *m_instance;
    int m_currentTimeMs = 0;
    struct TTimerEntry
    {
        int expireMs;
        bool peridic;
    };
    QMap<int, QMap<TimerForTestInterface*, TTimerEntry>> m_expireMap;
};

#endif // TIMERRUNNERFORTEST_H
