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
    QList<int> emptyExpireEntries;
    for(auto iter=m_expireMap.begin(); iter!=m_expireMap.end(); iter++) {
        QMap<TimerForTestInterface*, TTimerEntry> &expireList = iter.value();
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
            QMap<TimerForTestInterface*, TTimerEntry> &expireMap = iter.value();
            for(auto timerIter=expireMap.cbegin(); timerIter!=expireMap.cend(); timerIter++) {
                TTimerEntry entry = timerIter.value();
                if(!entry.singleShot) {
                    // TODO once periodic timers are implemented
                }
                timerIter.key()->fireExpired();
                QCoreApplication::processEvents();
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

