#include "singleshottimertest.h"

SingleShotTimerTest::SingleShotTimerTest(int expireTimeMs) :
    ZeraTimerTemplate(expireTimeMs)
{
}

SingleShotTimerTest::~SingleShotTimerTest()
{
    TimerRunnerForTest::getInstance()->removeTimer(this);
}

void SingleShotTimerTest::start()
{
    TimerRunnerForTest::getInstance()->addTimer(this, m_expireTimeMs, false);
}

void SingleShotTimerTest::stop()
{
    TimerRunnerForTest::getInstance()->removeTimer(this);
}

void SingleShotTimerTest::fireExpired()
{
    emit sigExpired();
}
