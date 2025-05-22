#ifndef SOURCESTATEPERIDICPOLLER_H
#define SOURCESTATEPERIDICPOLLER_H

#include "sourcetransactionstartnotifier.h"
#include "iogroupgenerator.h"
#include "idkeeper.h"
#include <timertemplateqt.h>
#include <QObject>
#include <QSharedPointer>

class SourceStatePeriodicPoller : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<SourceStatePeriodicPoller> Ptr;
    SourceStatePeriodicPoller(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationForQuery,
                              int pollTime = 500);
    void setPollTime(int ms);
    void startPeriodicPoll();
    void stopPeriodicPoll();
    bool tryStartPollNow();

private slots:
    void onPollTimer();
    void onStateQueryTransationStarted(int dataGroupId);
    void onResponseReceived(const IoQueueGroup::Ptr transferGroup);
protected:
    TimerTemplateQtPtr m_pollTimer;
private:
    SourceTransactionStartNotifier::Ptr m_sourceNotificationStateQuery;
    AbstractSourceIoPtr m_sourceIo;
    IoGroupGenerator m_ioGroupGenerator;
    IdKeeperMulti m_PendingStateQueryIds;
};

#endif // SOURCESTATEPERIDICPOLLER_H
