#ifndef SOURCEINTERACTORSTATUS_H
#define SOURCEINTERACTORSTATUS_H

#include "sourceioextserial.h"
#include "sourcetransactionstartnotifier.h"
#include "sourceperiodicpollerstate.h"
#include <QObject>

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
    void setPollCountAfterSwitchOnOk(int count);
signals:
    void sigStateChanged(States state);

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueGroup::Ptr transferGroup);
private:
    bool isErrorState() const;
    bool isNewError(const IoQueueGroup::Ptr transferGroup);
    void setState(States state);
    void setPollingOnStateChange();
    void handleSwitchResponse(const IoQueueGroup::Ptr transferGroup);
    void handleStateResponse(const IoQueueGroup::Ptr transferGroup);
    AbstractSourceIoPtr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    SourceStatePeriodicPoller::Ptr m_sourceStatePoller;
    States m_currState = States::UNDEFINED;
    int m_pollCountAfterSwitchOnOk = 1;
    int m_pollCountBeforeIdleOrError = 0;
    IdKeeperMulti m_pendingSwitchIds;
    IdKeeperMulti m_PendingStateQueryIds;
};

#endif // SOURCEINTERACTORSTATUS_H
