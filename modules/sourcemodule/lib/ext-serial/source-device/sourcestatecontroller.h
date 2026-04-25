#ifndef SOURCEINTERACTORSTATUS_H
#define SOURCEINTERACTORSTATUS_H

#include "sourcestates.h"
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
    void setPollCountAfterSwitchOnOk(int count);
signals:
    void sigStateChanged(SourceStates state);

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueGroup::Ptr transferGroup);
private:
    bool isErrorState() const;
    bool isNewError(const IoQueueGroup::Ptr transferGroup);
    void setState(SourceStates state);
    void setPollingOnStateChange();
    void handleSwitchResponse(const IoQueueGroup::Ptr transferGroup);
    void handleStateResponse(const IoQueueGroup::Ptr transferGroup);
    AbstractSourceIoPtr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    SourceStatePeriodicPoller::Ptr m_sourceStatePoller;
    SourceStates m_currState = SourceStates::UNDEFINED;
    int m_pollCountAfterSwitchOnOk = 1;
    int m_pollCountBeforeIdleOrError = 0;
    IdKeeperMulti m_pendingSwitchIds;
    IdKeeperMulti m_PendingStateQueryIds;
};

#endif // SOURCEINTERACTORSTATUS_H
