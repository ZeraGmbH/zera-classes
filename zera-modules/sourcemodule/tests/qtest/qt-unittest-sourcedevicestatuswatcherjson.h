#include <QObject>

#include "io-device/iodevicefactory.h"

class SourceDevice;

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
    SourceDevice *m_sourceDevice = nullptr;
    tIoDeviceShPtr m_interface;
};
