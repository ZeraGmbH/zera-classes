#include <QObject>

#include "io-device/iodevicefactory.h"
#include "sourcedevice-forunittest.h"


class SourceDeviceStatusWatcherJsonTest : public QObject
{
    Q_OBJECT
    virtual ~SourceDeviceStatusWatcherJsonTest();

private slots:
    void init();

    void statusPollAutoStart();

    void statusPollChangeTime();
    void initialBusyIdle();

    void busyOnCausesBusyOnOffState();
    void busyOnOffCausesBusyTwoOnOffState();

    void sequencePollSwitchErrorOnSwitch();
    void sequencePollSwitchErrorOnPoll();

    void sequenceSwitchPollErrorOnSwitch();
    void sequenceSwitchPollErrorOnPoll();

    void sequencePollSwitchErrorOnBoth();
    void sequenceSwitchPollErrorOnBoth();

    void sequencePollStopsOnError();
    void sequencePollStopsOnErrorAndStartsOnSwitch();
private:
    SourceDeviceForUnittest *m_sourceDevice = nullptr;
    tIoDeviceShPtr m_interface;
};
