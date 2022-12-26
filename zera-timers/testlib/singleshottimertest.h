#ifndef SINGLESHOTTIMERTEST_H
#define SINGLESHOTTIMERTEST_H

#include "zeratimertemplate.h"
#include "timerrunnerfortest.h"

class SingleShotTimerTest : public ZeraTimerTemplate, public TimerForTestTemplate
{
    Q_OBJECT
public:
    SingleShotTimerTest(int expireTimeMs);
    ~SingleShotTimerTest();
    void start() override;
    void stop() override;
    void fireExpired() override;
private:
    void removeFromRunner();
};

#endif // SINGLESHOTTIMERTEST_H
