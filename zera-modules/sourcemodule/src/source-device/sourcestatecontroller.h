#ifndef SOURCEINTERACTORSTATUS_H
#define SOURCEINTERACTORSTATUS_H

#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include <QObject>
#include <QTimer>

/*
 * Spawn periodic state poll
 * Observe source to generate state
 * notify current state changed
 */
class SourceStateController : public QObject
{
    Q_OBJECT
public:
    SourceStateController(ISourceIo *sourceIo,
                           SourceTransactionStartNotifier::Ptr sourceNotificationSwitch,
                           SourceTransactionStartNotifier::Ptr sourceNotificationStateQuery);
    enum class States
    {
        UNDEFINED,
        IDLE,
        SWITCH_BUSY,
        ERROR_SWITCH,
        ERROR_POLL
    };
    void setPollTime(int ms);
    bool tryStartPollNow();
    int isPeriodicPollActive() const;
    bool isErrorState() const;
signals:
    void sigStateChanged(States state);

private slots:
    void onPollTimer();
    void onSwitchTransactionStarted(int dataGroupId);
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueEntry::Ptr transferGroup);
private:
    void enablePolling();
    void disablePolling();
    void setState(States state);
    void setPollingOnStateChange();
    void handleSwitchResponse(const IoQueueEntry::Ptr transferGroup);
    void handleStateResponse(const IoQueueEntry::Ptr transferGroup);
    ISourceIo *m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    States m_currState = States::UNDEFINED;
    IdKeeperMulti m_pendingSwitchIds;
    IdKeeperMulti m_PendingStateQueryIds;
    QTimer m_pollTimer;
};

#endif // SOURCEINTERACTORSTATUS_H
