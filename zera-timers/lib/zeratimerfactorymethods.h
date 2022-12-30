#ifndef ZERATIMERFACTORYMETHODS_H
#define ZERATIMERFACTORYMETHODS_H

#include "zeratimertemplate.h"
#include <functional>

/* Use ZeraTimerFactoryMethods to create zera-timers - otherwise tests will fail
 * (see testlib/ZeraTimerFactoryMethodsTest why)
*/
class ZeraTimerFactoryMethods
{
public:
    static ZeraTimerTemplatePtr createSingleShot(int timeout);
protected:
    static std::function<ZeraTimerTemplatePtr(int)> m_singleShotCreateFunction;
};

#endif // ZERATIMERFACTORYMETHODS_H
