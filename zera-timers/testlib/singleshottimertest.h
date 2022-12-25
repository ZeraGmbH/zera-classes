#ifndef SINGLESHOTTIMERTEST_H
#define SINGLESHOTTIMERTEST_H

#include "zeratimertemplate.h"
#include "timerrunnerfortest.h"

class SingleShotTimerTest : public ZeraTimerTemplate, public TimerForTestInterface
{
    Q_OBJECT
public:
    SingleShotTimerTest(int expireTimeMs);
    void start() override;
    void stop() override;
    void fireExpired() override;
    ~SingleShotTimerTest();
};

#endif // SINGLESHOTTIMERTEST_H
