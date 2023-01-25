#include "zeratimerfactorymethods.h"
#include "singleshottimerqt.h"
#include "periodictimerqt.h"

std::function<ZeraTimerTemplatePtr(int)> ZeraTimerFactoryMethods::m_singleShotCreateFunction = [](int timeout){
    return std::make_unique<SingleShotTimerQt>(timeout);
};

std::function<ZeraTimerTemplatePtr(int)> ZeraTimerFactoryMethods::m_periodicCreateFunction = [](int timeout){
    return std::make_unique<PeriodicTimerQt>(timeout);
};


ZeraTimerTemplatePtr ZeraTimerFactoryMethods::createSingleShot(int timeout)
{
    return m_singleShotCreateFunction(timeout);
}

ZeraTimerTemplatePtr ZeraTimerFactoryMethods::createPeriodic(int timeout)
{
    return m_periodicCreateFunction(timeout);
}
