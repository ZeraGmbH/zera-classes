#include "zeratimerfactorymethodstest.h"
#include "singleshottimertest.h"
#include "periodictimertest.h"

void ZeraTimerFactoryMethodsTest::enableTest()
{
    m_singleShotCreateFunction = [](int timeout){
        return std::make_unique<SingleShotTimerTest>(timeout);
    };

    m_periodicCreateFunction = [](int timeout){
        return std::make_unique<PeriodicTimerTest>(timeout);
    };
}
