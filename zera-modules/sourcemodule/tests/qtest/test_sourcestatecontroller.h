#include <QObject>

#include "iodevicefactory.h"
#include "sourcedeviceerrorinjection-forunittest.h"

class test_sourcestatecontroller : public QObject
{
    Q_OBJECT
public slots:
    void onIoQueueGroupFinished(IoQueueGroup::Ptr workGroup);

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
    void sequenceSwitchPollErrorOnPostPoll();

    void sequencePollSwitchErrorOnBoth();
    void sequenceSwitchPollErrorOnBoth();

    void pollStopsAfterSwitchError();
    void pollStopsAfterPollError();
    void pollStopsAfterErrorAndRestartsAfterSuccessfulSwitch();
private:
    void setDemoResonseErrorIdx(int idx);
    ISourceIo::Ptr m_sourceIo;
    ISourceIo::Ptr m_sourceIoWithError;
    IoDeviceBase::Ptr m_ioDevice;
    IoQueueGroupListPtr m_listIoGroupsReceived;
};
