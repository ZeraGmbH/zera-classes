#include "sourcestatecontroller.h"

SourceStateController::SourceStateController(SourceTransactionStartNotifier::Ptr sourceNotificationSwitch,
                                             SourceTransactionStartNotifier::Ptr sourceNotificationStateQuery,
                                             SourceStatePeriodicPoller::Ptr sourceStatePoller) :
    m_sourceIo(sourceNotificationSwitch->getSourceIo()),
    m_sourceNotificationSwitch(sourceNotificationSwitch),
    m_sourceNotificationStateQuery(sourceNotificationStateQuery),
    m_sourceStatePoller(sourceStatePoller)
{
    connect(m_sourceNotificationSwitch.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onSwitchTransactionStarted);
    connect(m_sourceNotificationStateQuery.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onStateQueryTransationStarted);
    connect(m_sourceIo.get(), &SourceIo::sigResponseReceived,
            this, &SourceStateController::onResponseReceived);
}

bool SourceStateController::isErrorState() const
{
    return m_currState == States::ERROR_SWITCH ||
           m_currState == States::ERROR_POLL;
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
        m_sourceStatePoller->startPeriodicPoll();
    }
    else {
        m_sourceStatePoller->stopPeriodicPoll();
    }
}

void SourceStateController::handleSwitchResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(isNewError(transferGroup)) {
        setState(States::ERROR_SWITCH);
    }
    else if(m_currState == States::SWITCH_BUSY) {
        setState(States::IDLE);
    }
}

void SourceStateController::handleStateResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(isNewError(transferGroup)) {
        setState(States::ERROR_POLL);
    }
    else if(m_currState == States::UNDEFINED) {
        setState(States::IDLE);
    }
}

bool SourceStateController::isNewError(const IoQueueEntry::Ptr transferGroup)
{
    return !transferGroup->passedAll() && !isErrorState();
}
