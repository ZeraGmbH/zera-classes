#include "zeratimerfactorymethodstest.h"
#include "singleshottimertest.h"

void ZeraTimerFactoryMethodsTest::enableTest()
{
    m_singleShotCreateFunction = [](int timeout){
        return std::make_unique<SingleShotTimerTest>(timeout);
    };
}
