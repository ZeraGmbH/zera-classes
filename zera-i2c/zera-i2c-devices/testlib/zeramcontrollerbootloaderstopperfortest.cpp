#include "zeramcontrollerbootloaderstopperfortest.h"
#include <timerfactoryqt.h>

ZeraMControllerBootloaderStopperForTest::ZeraMControllerBootloaderStopperForTest(int channelId, bool assumeAppStartedImmediate) :
     m_channelId(channelId),
     m_assumeAppStartedImmediate(assumeAppStartedImmediate)
{
}

void ZeraMControllerBootloaderStopperForTest::stopBootloader(int msWaitForApplicationStart)
{
    if(m_assumeAppStartedImmediate)
        emit sigAssumeBootloaderStopped(m_channelId);
    else {
        m_appWaitSimulTimer = TimerFactoryQt::createSingleShot(msWaitForApplicationStart);
        connect(m_appWaitSimulTimer.get(), &TimerTemplateQt::sigExpired,
                this, &ZeraMControllerBootloaderStopperForTest::onWaitTimerExpired);
        m_appWaitSimulTimer->start();
    }
}

void ZeraMControllerBootloaderStopperForTest::onWaitTimerExpired()
{
    emit sigAssumeBootloaderStopped(m_channelId);
}
