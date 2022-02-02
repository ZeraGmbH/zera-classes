#include <QObject>

#include "io-device/iodevicefactory.h"
#include "sourcedeviceerrorinjection-forunittest.h"

class SourceStateControllerTest : public QObject
{
    Q_OBJECT
    virtual ~SourceStateControllerTest();

public slots:
    void onIoQueueGroupFinished(IoQueueEntry::Ptr workGroup);

private slots:
    void init();

    void statePollAutoStart();

    void statePollChangeTime();
    void stateInitialIdle();

    void switchOnCausesBusyOnOffState();
    void switchOnOffCausesBusyTwoOnOffState();

    void sequencePollSwitchErrorOnSwitch();
    void sequencePollSwitchErrorOnPoll();

    void sequenceSwitchPollErrorOnSwitch();
    void sequenceSwitchPollErrorOnPoll();

    void sequencePollSwitchErrorOnBoth();
    void sequenceSwitchPollErrorOnBoth();

    void pollStopsAfterSwitchError();
    void pollStopsAfterPollError();
    void pollStopsAfterErrorAndRestartsAfterSuccessfulSwitch();
private:
    SourceDeviceErrorInjection *m_sourceDevice = nullptr;
    SourceIo *m_sourceDeviceUnderTest = nullptr;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueueEntryList m_listIoGroupsReceived;
};
