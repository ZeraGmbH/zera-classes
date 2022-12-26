#include "timerrunnerfortest.h"

void TimerForTestTemplate::setRunner(TimerRunnerForTest *timeRunner)
{
    m_timeRunner = timeRunner;
}


void TimerRunnerForTest::addTimer(TimerForTestTemplate *timer, int expiredMs, bool singleShot)
{
    removeTimer(timer);
    int expireTime = calcExpireTime(expiredMs);
    if(!m_expireMap.contains(expireTime))
        m_expireMap[expireTime] = QMap<TimerForTestTemplate*, TTimerEntry>();
    m_expireMap[expireTime][timer] = TTimerEntry({expiredMs, singleShot});
}

void TimerRunnerForTest::removeTimer(TimerForTestTemplate *timer)
{
    QList<int> emptyExpireEntries;
    for(auto iter=m_expireMap.begin(); iter!=m_expireMap.end(); iter++) {
        QMap<TimerForTestTemplate*, TTimerEntry> &expireList = iter.value();
        expireList.remove(timer);
        if(expireList.isEmpty())
            emptyExpireEntries.append(iter.key());
    }
    for(auto expireTimeEmpty : emptyExpireEntries)
        m_expireMap.remove(expireTimeEmpty);
}

void TimerRunnerForTest::processTimers(int durationMs)
{
    QList<int> expireEntriesToRemove;
    int nextCurrentTimeMs = calcExpireTime(durationMs);
    for(auto iter=m_expireMap.begin(); iter!=m_expireMap.end(); iter++) {
        int entryExpireTime = iter.key();
        if(entryExpireTime <= nextCurrentTimeMs) {
            m_currentTimeMs = entryExpireTime;
            expireEntriesToRemove.append(entryExpireTime);
            QMap<TimerForTestTemplate*, TTimerEntry> &expireMap = iter.value();
            for(auto timerIter=expireMap.cbegin(); timerIter!=expireMap.cend(); timerIter++) {
                TTimerEntry entry = timerIter.value();
                if(!entry.singleShot) {
                    // TODO once periodic timers are implemented
                }
                timerIter.key()->fireExpired();
            }
        }
    }
    for(auto entryToRemove : expireEntriesToRemove)
        m_expireMap.remove(entryToRemove);
    m_currentTimeMs = nextCurrentTimeMs;
}

int TimerRunnerForTest::getCurrentTimeMs()
{
    return m_currentTimeMs;
}

int TimerRunnerForTest::calcExpireTime(int expiredMs)
{
    return m_currentTimeMs + expiredMs;
}

