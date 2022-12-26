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
    static TimerRunnerForTest* getInstance();
    static void reset();
    void addTimer(TimerForTestInterface* timer, int expiredMs, bool singleShot);
    void removeTimer(TimerForTestInterface* timer);
    void processTimers(int durationMs);
    int getCurrentTimeMs();
private:
    TimerRunnerForTest() = default;
    int calcExpireTime(int expiredMs);
    int m_currentTimeMs = 0;
    struct TTimerEntry
    {
        int expireMs;
        bool singleShot;
    };
    QMap<int, QMap<TimerForTestInterface*, TTimerEntry>> m_expireMap;
    static TimerRunnerForTest* m_instance;
};

#endif // TIMERRUNNERFORTEST_H
