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
    SourceIoErrorInjection *m_sourceIo = nullptr;
    ISourceIo::Ptr m_sourceIoPtr;
    SourceIo *m_sourceIoUnderTest = nullptr;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueueEntryList m_listIoGroupsReceived;
};
