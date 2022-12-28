#include "timerrunnerfortest.h"
#include <QCoreApplication>

TimerRunnerForTest *TimerRunnerForTest::m_instance = nullptr;

TimerRunnerForTest *TimerRunnerForTest::getInstance()
{
    if(m_instance == nullptr)
        m_instance = new TimerRunnerForTest;
    return m_instance;
}

void TimerRunnerForTest::reset()
{
    delete m_instance;
    m_instance = nullptr;
}

int TimerRunnerForTest::getCurrentTimeMs()
{
    return m_currentTimeMs;
}

void TimerRunnerForTest::addTimer(TimerForTestInterface *timer, int expiredMs, bool singleShot)
{
    removeTimer(timer);
    int expireTime = calcExpireTime(expiredMs);
    if(!m_expireMap.contains(expireTime))
        m_expireMap[expireTime] = QMap<TimerForTestInterface*, TTimerEntry>();
    m_expireMap[expireTime][timer] = TTimerEntry({expiredMs, singleShot});
}

void TimerRunnerForTest::removeTimer(TimerForTestInterface *timer)
{
    QList<int> emptyEntries;
    for(auto iter=m_expireMap.begin(); iter!=m_expireMap.end(); iter++) {
        ExpireEntries &expireList = iter.value();
        expireList.remove(timer);
        if(expireList.isEmpty())
            emptyEntries.append(iter.key());
    }
    removeTimers(emptyEntries);
}

void TimerRunnerForTest::processTimers(int durationMs)
{
    Q_ASSERT(durationMs >= 0);

    int nextCurrentTimeMs = calcExpireTime(durationMs);
    ExpireMap expiredMap = getMapToProcess(nextCurrentTimeMs);

    bool startedOrProcessed = false;
    if(processExpiredTimers(expiredMap))
        startedOrProcessed = true;
    else if(tryStartTimersByEventLoop())
        startedOrProcessed = true;
    if(startedOrProcessed)
        processTimers(nextCurrentTimeMs - m_currentTimeMs);
    else
        m_currentTimeMs = nextCurrentTimeMs;
}

TimerRunnerForTest::ExpireMap TimerRunnerForTest::getMapToProcess(int upToTimestamp)
{
    ExpireMap expiredMap;
    for(auto iter=m_expireMap.cbegin(); iter!=m_expireMap.cend(); iter++) {
        int entryExpireTime = iter.key();
        if(entryExpireTime <= upToTimestamp)
            expiredMap[entryExpireTime] = iter.value();
    }
    return expiredMap;
}

bool TimerRunnerForTest::processExpiredTimers(const ExpireMap &map)
{
    bool timerFired = false;
    QList<int> expiredTimes;
    for(auto iter=map.cbegin(); iter!=map.cend(); iter++) {
        int entryExpireTime = iter.key();
        expiredTimes.append(entryExpireTime);
        m_currentTimeMs = entryExpireTime;
        const ExpireEntries &expireMap = iter.value();
        for(auto timerIter=expireMap.cbegin(); timerIter!=expireMap.cend(); timerIter++) {
            TTimerEntry entry = timerIter.value();
            if(!entry.singleShot) {
                // TODO once periodic timers are implemented
            }
            timerFired = true;
            timerIter.key()->fireExpired();
            QCoreApplication::processEvents();
        }
    }
    removeTimers(expiredTimes);
    return timerFired;
}

bool TimerRunnerForTest::tryStartTimersByEventLoop()
{
    int countTimersBeforeEventLoop = m_expireMap.count();
    QCoreApplication::processEvents();
    int countTimersAfterEventLoop = m_expireMap.count();
    return countTimersAfterEventLoop > countTimersBeforeEventLoop;
}

void TimerRunnerForTest::removeTimers(const QList<int> &expiredTimes)
{
    for(int expireTime : expiredTimes)
        m_expireMap.remove(expireTime);
}

int TimerRunnerForTest::calcExpireTime(int expiredMs)
{
    return m_currentTimeMs + expiredMs;
}
