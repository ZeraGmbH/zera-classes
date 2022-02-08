#ifndef SOURCESTATEPERIDICPOLLER_H
#define SOURCESTATEPERIDICPOLLER_H

#include "sourcetransactionstartnotifier.h"
#include <QObject>
#include <QTimer>

class SourceStatePeriodicPoller : public QObject
{
    Q_OBJECT
public:
    SourceStatePeriodicPoller(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationForQuery,
                              int pollTime = 500);
    void setPollTime(int ms);
    void startPeriodicPoll();
    void stopPeriodicPoll();
    bool tryStartPollNow();
    int isPeriodicPollActive() const;

private slots:
    void onPollTimer();
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueEntry::Ptr transferGroup);
private:
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    ISourceIo::Ptr m_sourceIo;
    QTimer m_pollTimer;
    IdKeeperMulti m_PendingStateQueryIds;
};

#endif // SOURCESTATEPERIDICPOLLER_H
