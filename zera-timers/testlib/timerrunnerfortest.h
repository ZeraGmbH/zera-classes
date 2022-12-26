#ifndef TIMERRUNNERFORTEST_H
#define TIMERRUNNERFORTEST_H

#include <QMap>
#include <QList>

class TimerRunnerForTest;
class TimerForTestTemplate
{
public:
    virtual void fireExpired() = 0;
    void setRunner(TimerRunnerForTest* timeRunner);
protected:
    TimerRunnerForTest* m_timeRunner;
};

class TimerRunnerForTest
{
public:
    void addTimer(TimerForTestTemplate* timer, int expiredMs, bool singleShot);
    void removeTimer(TimerForTestTemplate* timer);
    void processTimers(int durationMs);
    int getCurrentTimeMs();
private:
    int calcExpireTime(int expiredMs);
    int m_currentTimeMs = 0;
    struct TTimerEntry
    {
        int expireMs;
        bool singleShot;
    };
    QMap<int, QMap<TimerForTestTemplate*, TTimerEntry>> m_expireMap;
};

#endif // TIMERRUNNERFORTEST_H
