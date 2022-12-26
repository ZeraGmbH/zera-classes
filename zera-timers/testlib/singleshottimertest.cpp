#include "singleshottimertest.h"

SingleShotTimerTest::SingleShotTimerTest(int expireTimeMs) :
    ZeraTimerTemplate(expireTimeMs)
{
}

SingleShotTimerTest::~SingleShotTimerTest()
{
    removeFromRunner();
}

void SingleShotTimerTest::start()
{
    TimerRunnerForTest::getInstance()->addTimer(this, m_expireTimeMs, true);
}

void SingleShotTimerTest::stop()
{
    removeFromRunner();
}

void SingleShotTimerTest::fireExpired()
{
    emit sigExpired();
}

void SingleShotTimerTest::removeFromRunner()
{
    TimerRunnerForTest::getInstance()->removeTimer(this);
}
