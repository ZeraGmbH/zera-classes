#include "sourceperiodicpollerstate.h"
#include "jsonstructureloader.h"
#include <timerfactoryqt.h>

SourceStatePeriodicPoller::SourceStatePeriodicPoller(SourceTransactionStartNotifier::Ptr sourceIoWithNotificationQuery, int pollTime) :
    m_pollTimer(TimerFactoryQt::createPeriodic(pollTime)),
    m_sourceNotificationStateQuery(sourceIoWithNotificationQuery),
    m_sourceIo(m_sourceNotificationStateQuery->getSourceIo()),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(m_sourceIo->getProperties()))
{
    connect(m_pollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SourceStatePeriodicPoller::onPollTimer);
    connect(m_sourceNotificationStateQuery.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStatePeriodicPoller::onStateQueryTransationStarted);
    connect(m_sourceIo.get(), &AbstractSourceIo::sigResponseReceived,
            this, &SourceStatePeriodicPoller::onResponseReceived);
    startPeriodicPoll();
}

void SourceStatePeriodicPoller::setPollTime(int ms)
{
    m_pollTimer = TimerFactoryQt::createPeriodic(ms);
    connect(m_pollTimer.get(), &TimerTemplateQt::sigExpired,
            this, &SourceStatePeriodicPoller::onPollTimer);
    m_pollTimer->start();
}

void SourceStatePeriodicPoller::startPeriodicPoll()
{
    m_pollTimer->start();
}

void SourceStatePeriodicPoller::stopPeriodicPoll()
{
    m_pollTimer->stop();
}

bool SourceStatePeriodicPoller::tryStartPollNow()
{
    bool bStarted = false;
    if(!m_PendingStateQueryIds.hasPending()) {
        IoQueueGroup::Ptr transferGroup = m_ioGroupGenerator.generateStatusPollGroup();
        m_sourceNotificationStateQuery->startTransactionWithNotify(transferGroup);
        bStarted = true;
    }
    return bStarted;
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
