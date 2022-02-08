#include "sourcestatecontroller.h"

SourceStateController::SourceStateController(ISourceIo::Ptr sourceIo,
                                               SourceTransactionStartNotifier::Ptr sourceNotificationSwitch,
                                               SourceTransactionStartNotifier::Ptr sourceNotificationStateQuery) :
    m_sourceIo(sourceIo),
    m_sourceNotificationSwitch(sourceNotificationSwitch),
    m_sourceNotificationStateQuery(sourceNotificationStateQuery)

{
    m_pollTimer.setSingleShot(false);
    m_pollTimer.setInterval(500);
    enablePolling();

    connect(&m_pollTimer, &QTimer::timeout,
            this, &SourceStateController::onPollTimer);

    connect(m_sourceNotificationSwitch.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onSwitchTransactionStarted);
    connect(m_sourceNotificationStateQuery.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onStateQueryTransationStarted);

    connect(m_sourceIo.get(), &SourceIo::sigResponseReceived,
            this, &SourceStateController::onResponseReceived);
}

void SourceStateController::setPollTime(int ms)
{
    bool active = m_pollTimer.isActive();
    if(active) {
        disablePolling();
    }
    m_pollTimer.setInterval(ms);
    if(active) {
        enablePolling();
    }
}

bool SourceStateController::tryStartPollNow()
{
    bool bStarted = false;
    if(!m_PendingStateQueryIds.hasPending()) {
        IoQueueEntry::Ptr transferGroup = m_sourceIo->getIoGroupGenerator().generateStatusPollGroup();
        m_sourceNotificationStateQuery->startTransactionWithNotify(transferGroup);
        bStarted = true;
    }
    return bStarted;
}

int SourceStateController::isPeriodicPollActive() const
{
    return m_pollTimer.isActive();
}

bool SourceStateController::isErrorState() const
{
    return m_currState == States::ERROR_SWITCH ||
            m_currState == States::ERROR_POLL;
}

void SourceStateController::onPollTimer()
{
    tryStartPollNow();
}

void SourceStateController::onSwitchTransactionStarted(int dataGroupId)
{
    m_pendingSwitchIds.setPending(dataGroupId);
    setState(States::SWITCH_BUSY);
}

void SourceStateController::onStateQueryTransationStarted(int dataGroupId)
{
    m_PendingStateQueryIds.setPending(dataGroupId);
}

void SourceStateController::onResponseReceived(const IoQueueEntry::Ptr transferGroup)
{
    int groupId = transferGroup->getGroupId();
    if(m_pendingSwitchIds.isPendingAndRemoveIf(groupId)) {
        handleSwitchResponse(transferGroup);
    }
    if(m_PendingStateQueryIds.isPendingAndRemoveIf(groupId)) {
        handleStateResponse(transferGroup);
    }
}

void SourceStateController::enablePolling()
{
    m_pollTimer.start();
}

void SourceStateController::disablePolling()
{
    m_pollTimer.stop();
}

void SourceStateController::setState(States state)
{
    if(m_currState != state) {
        m_currState = state;
        emit sigStateChanged(state);
        setPollingOnStateChange();
    }
}

void SourceStateController::setPollingOnStateChange()
{
    if(!isErrorState()) {
        enablePolling();
    }
    else {
        disablePolling();
    }
}

void SourceStateController::handleSwitchResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(!transferGroup->passedAll() && !isErrorState()) {
        setState(States::ERROR_SWITCH);
    }
    else if(m_currState == States::SWITCH_BUSY) {
        setState(States::IDLE);
    }
}

void SourceStateController::handleStateResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(!transferGroup->passedAll() && !isErrorState()) {
        setState(States::ERROR_POLL);
    }
    else if(m_currState == States::UNDEFINED) {
        setState(States::IDLE);
    }
}
