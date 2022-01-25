#ifndef SOURCEINTERACTORSTATUS_H
#define SOURCEINTERACTORSTATUS_H

#include "sourcedevice.h"
#include "sourcetransactionstartnotifier.h"
#include <QObject>
#include <QTimer>

/*
 * Spawn periodic state poll
 * Observe source to generate state
 * notify current state changed
 */

enum class SourceStates
{
    UNDEFINED,
    IDLE,
    SWITCH_BUSY,
    ERROR_SWITCH,
    ERROR_POLL
};

class SourceStateQueries
{
public:
    bool isUnequal(SourceStates state) { return m_state != state; }
    bool isIdle() { return m_state==SourceStates::IDLE; }
    bool isError() { return m_state==SourceStates::ERROR_SWITCH || m_state==SourceStates::ERROR_POLL; }
    bool isSwitchBusy() { return m_state==SourceStates::SWITCH_BUSY; }
    bool isUndefined() { return m_state==SourceStates::UNDEFINED; }

    void setState(SourceStates state) { m_state = state; }
private:
    SourceStates m_state = SourceStates::UNDEFINED;
};


class SourceStateController : public QObject
{
    Q_OBJECT
public:
    SourceStateController(ISourceDevice *sourceDevice,
                           SourceTransactionStartNotifier *sourceNotificationSwitch,
                           SourceTransactionStartNotifier *sourceNotificationStateQuery);

    void setPollTime(int ms);
    bool tryStartPollNow();
    int isPeriodicPollActive();

signals:
    void sigStateChanged(SourceStates state);

private slots:
    void onPollTimer();
    void onSwitchTransactionStarted(int dataGroupId);
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoTransferDataGroup::Ptr transferGroup);
private:
    void enablePolling();
    void disablePolling();
    void setState(SourceStates state);
    void setPollingOnStateChange();
    void handleSwitchResponse(const IoTransferDataGroup::Ptr transferGroup);
    void handleStateResponse(const IoTransferDataGroup::Ptr transferGroup);

    ISourceDevice *m_sourceDevice;
    SourceTransactionStartNotifier *m_sourceNotificationSwitch;
    SourceTransactionStartNotifier *m_sourceNotificationStateQuery;
    SourceStateQueries m_currState;
    IdKeeperMulti m_pendingSwitchIds;
    IdKeeperMulti m_PendingStateQueryIds;

    QTimer m_pollTimer;
};

#endif // SOURCEINTERACTORSTATUS_H
