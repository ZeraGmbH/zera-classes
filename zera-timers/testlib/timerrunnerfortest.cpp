#include "timerrunnerfortest.h"

TimerRunnerForTest *TimerRunnerForTest::m_instance = nullptr;

TimerRunnerForTest *TimerRunnerForTest::getInstance()
{
    if(m_instance == nullptr)
        m_instance = new TimerRunnerForTest;
    return m_instance;
}

void TimerRunnerForTest::addTimer(TimerForTestInterface *timer, int expiredMs, bool periodic)
{
    removeTimer(timer);
    int expireTime = calcExpireTime(expiredMs);
    if(!m_expireMap.contains(expireTime))
        m_expireMap[expireTime] = QMap<TimerForTestInterface*, TTimerEntry>();
    m_expireMap[expireTime][timer] = TTimerEntry({expiredMs, periodic});
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

}

int TimerRunnerForTest::getCurrentTimeMs()
{
    return m_currentTimeMs;
}

TimerRunnerForTest::TimerRunnerForTest()
{
}

int TimerRunnerForTest::calcExpireTime(int expiredMs)
{
    return m_currentTimeMs + expiredMs;
}
