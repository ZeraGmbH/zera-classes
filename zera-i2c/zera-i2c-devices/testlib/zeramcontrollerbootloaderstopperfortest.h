#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPERFORTEST_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPERFORTEST_H

#include "zeramcontrollerio.h"
#include "zeramcontrollerbootloaderstopperinterface.h"
#include <timertemplateqt.h>

class ZeraMControllerBootloaderStopperForTest : public ZeraMControllerBootloaderStopperInterface
{
    Q_OBJECT
public:
    ZeraMControllerBootloaderStopperForTest(int channelId, bool assumeAppStartedImmediate);
    void stopBootloader(int msWaitForApplicationStart = 1000) override;
private slots:
    void onWaitTimerExpired();
private:
    int m_channelId;
    bool m_assumeAppStartedImmediate;
    TimerTemplateQtPtr m_appWaitSimulTimer;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPERFORTEST_H
