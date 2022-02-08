#ifndef SOURCEINTERACTORSTATUS_H
#define SOURCEINTERACTORSTATUS_H

#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include "sourceperiodicpollerstate.h"
#include <QObject>
#include <QTimer>

/*
 * Observe source to generate state change notification
 */
class SourceStateController : public QObject
{
    Q_OBJECT
public:
    SourceStateController(SourceTransactionStartNotifier::Ptr sourceNotificationSwitch,
                          SourceTransactionStartNotifier::Ptr sourceNotificationStateQuery,
                          SourceStatePeriodicPoller::Ptr sourceStatePoller);
    enum class States
    {
        UNDEFINED,
        IDLE,
        SWITCH_BUSY,
        ERROR_SWITCH,
        ERROR_POLL
    };
    bool isErrorState() const;
signals:
    void sigStateChanged(States state);

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueEntry::Ptr transferGroup);
private:
    void setState(States state);
    void setPollingOnStateChange();
    void handleSwitchResponse(const IoQueueEntry::Ptr transferGroup);
    void handleStateResponse(const IoQueueEntry::Ptr transferGroup);
    ISourceIo::Ptr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    SourceStatePeriodicPoller::Ptr m_sourceStatePoller;
    States m_currState = States::UNDEFINED;
    IdKeeperMulti m_pendingSwitchIds;
    IdKeeperMulti m_PendingStateQueryIds;
};

#endif // SOURCEINTERACTORSTATUS_H
