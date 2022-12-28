#include "zeratimerfactorymethods.h"
#include "singleshottimerqt.h"

std::function<ZeraTimerTemplatePtr(int)> ZeraTimerFactoryMethods::m_singleShotCreateFunction = [](int timeout){
    return std::make_unique<SingleShotTimerQt>(timeout);
};

ZeraTimerTemplatePtr ZeraTimerFactoryMethods::createSingleShot(int timeout)
{
    return m_singleShotCreateFunction(timeout);
}
