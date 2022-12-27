#ifndef TIMERRUNNERFORTEST_H
#define TIMERRUNNERFORTEST_H

#include "timertestdefaults.h"
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
    struct TTimerEntry
    {
        int expireMs;
        bool singleShot;
    };
    typedef QMap<TimerForTestInterface*, TTimerEntry> ExpireEntries;
    typedef QMap<int/*expireTimeMs*/, ExpireEntries> ExpireMap;
    void processExpiredTimers(const ExpireMap &map);
    void removeTimers(const QList<int> &expiredTimes);
    int m_currentTimeMs = 0;
    ExpireMap m_expireMap;
    static TimerRunnerForTest* m_instance;
};

#endif // TIMERRUNNERFORTEST_H
