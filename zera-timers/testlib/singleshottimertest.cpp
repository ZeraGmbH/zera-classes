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
    m_timeRunner->addTimer(this, m_expireTimeMs, true);
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
    m_timeRunner->removeTimer(this);
}
