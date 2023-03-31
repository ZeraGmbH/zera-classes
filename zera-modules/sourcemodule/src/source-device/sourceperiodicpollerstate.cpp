#include "sourceperiodicpollerstate.h"

SourceStatePeriodicPoller::SourceStatePeriodicPoller(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationQuery, int pollTime) :
    m_sourceNotificationStateQuery(sourceIoWithNotificationQuery),
    m_sourceIo(m_sourceNotificationStateQuery->getSourceIo())

{
    m_pollTimer.setSingleShot(false);
    m_pollTimer.setInterval(pollTime);
    connect(&m_pollTimer, &QTimer::timeout,
            this, &SourceStatePeriodicPoller::onPollTimer);
    connect(m_sourceNotificationStateQuery.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStatePeriodicPoller::onStateQueryTransationStarted);
    connect(m_sourceIo.get(), &SourceIo::sigResponseReceived,
            this, &SourceStatePeriodicPoller::onResponseReceived);
    startPeriodicPoll();
}

void SourceStatePeriodicPoller::setPollTime(int ms)
{
    bool active = m_pollTimer.isActive();
    if(active) {
        stopPeriodicPoll();
    }
    m_pollTimer.setInterval(ms);
    if(active) {
        startPeriodicPoll();
    }
}

void SourceStatePeriodicPoller::startPeriodicPoll()
{
    m_pollTimer.start();
}

void SourceStatePeriodicPoller::stopPeriodicPoll()
{
    m_pollTimer.stop();
}

bool SourceStatePeriodicPoller::tryStartPollNow()
{
    bool bStarted = false;
    if(!m_PendingStateQueryIds.hasPending()) {
        IoQueueGroup::Ptr transferGroup = m_sourceIo->getIoGroupGenerator().generateStatusPollGroup();
        m_sourceNotificationStateQuery->startTransactionWithNotify(transferGroup);
        bStarted = true;
    }
    return bStarted;
}

int SourceStatePeriodicPoller::isPeriodicPollActive() const
{
    return m_pollTimer.isActive();
}

void SourceStatePeriodicPoller::onPollTimer()
{
    tryStartPollNow();
}

void SourceStatePeriodicPoller::onStateQueryTransationStarted(int dataGroupId)
{
    m_PendingStateQueryIds.setPending(dataGroupId);
}

void SourceStatePeriodicPoller::onResponseReceived(const IoQueueGroup::Ptr transferGroup)
{
    m_PendingStateQueryIds.isPendingAndRemoveIf(transferGroup->getGroupId());
}
