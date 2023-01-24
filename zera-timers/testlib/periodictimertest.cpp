#include "periodictimertest.h"

PeriodicTimerTest::PeriodicTimerTest(int expireTimeMs) :
    ZeraTimerTemplate(expireTimeMs)
{
}

PeriodicTimerTest::~PeriodicTimerTest()
{
    removeFromRunner();
}

void PeriodicTimerTest::start()
{
    TimerRunnerForTest::getInstance()->addTimer(this, m_expireTimeMs, false);
}

void PeriodicTimerTest::stop()
{
    removeFromRunner();
}

void PeriodicTimerTest::fireExpired()
{
    emit sigExpired();
}

void PeriodicTimerTest::removeFromRunner()
{
    TimerRunnerForTest::getInstance()->removeTimer(this);
}
