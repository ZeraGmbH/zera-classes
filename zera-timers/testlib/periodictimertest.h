#ifndef PERIODICTIMERTEST_H
#define PERIODICTIMERTEST_H

#include "zeratimertemplate.h"
#include "timerrunnerfortest.h"

class PeriodicTimerTest : public ZeraTimerTemplate, public TimerForTestInterface
{
    Q_OBJECT
public:
    PeriodicTimerTest(int expireTimeMs);
    ~PeriodicTimerTest();
    void start() override;
    void stop() override;
    void fireExpired() override;
private:
    void removeFromRunner();
};

#endif // PERIODICTIMERTEST_H
