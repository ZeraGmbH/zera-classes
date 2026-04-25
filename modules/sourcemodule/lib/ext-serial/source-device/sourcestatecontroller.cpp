#include "sourcestatecontroller.h"
#include "sourceioextserial.h"

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
    connect(m_sourceIo.get(), &SourceIoExtSerial::sigResponseReceived,
            this, &SourceStateController::onResponseReceived);
}

void SourceStateController::setPollCountAfterSwitchOnOk(int count)
{
    m_pollCountAfterSwitchOnOk = count;
}

bool SourceStateController::isErrorState() const
{
    return m_currState == SourceStates::ERROR_SWITCH ||
           m_currState == SourceStates::ERROR_POLL;
}

void SourceStateController::onSwitchTransactionStarted(int dataGroupId)
{
    m_pendingSwitchIds.setPending(dataGroupId);
    setState(SourceStates::SWITCH_BUSY);
}

void SourceStateController::onStateQueryTransationStarted(int dataGroupId)
{
    m_PendingStateQueryIds.setPending(dataGroupId);
}

void SourceStateController::onResponseReceived(const IoQueueGroup::Ptr transferGroup)
{
    int groupId = transferGroup->getGroupId();
    if(m_pendingSwitchIds.isPendingAndRemoveIf(groupId)) {
        handleSwitchResponse(transferGroup);
    }
    if(m_PendingStateQueryIds.isPendingAndRemoveIf(groupId)) {
        handleStateResponse(transferGroup);
    }
}

void SourceStateController::setState(SourceStates state)
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

void SourceStateController::handleSwitchResponse(const IoQueueGroup::Ptr transferGroup)
{
    if(isNewError(transferGroup)) {
        setState(SourceStates::ERROR_SWITCH);
    }
    else if(m_currState == SourceStates::SWITCH_BUSY) {
        if(m_pollCountAfterSwitchOnOk > 0) {
            m_pollCountBeforeIdleOrError = m_pollCountAfterSwitchOnOk;
            m_sourceStatePoller->startPeriodicPoll();
        }
        else {
            setState(SourceStates::IDLE);
        }
    }
}

void SourceStateController::handleStateResponse(const IoQueueGroup::Ptr transferGroup)
{
    if(m_pollCountBeforeIdleOrError > 0) {
        m_pollCountBeforeIdleOrError--;
        if(isNewError(transferGroup)) {
            setState(SourceStates::ERROR_SWITCH);
            m_pollCountBeforeIdleOrError = 0;
        }
        else if(m_pollCountBeforeIdleOrError == 0) {
            setState(SourceStates::IDLE);
        }
    }
    else if(isNewError(transferGroup)) {
        setState(SourceStates::ERROR_POLL);
    }
    else if(m_currState == SourceStates::UNDEFINED) {
        setState(SourceStates::IDLE);
    }
}

bool SourceStateController::isNewError(const IoQueueGroup::Ptr transferGroup)
{
    return !transferGroup->passedAll() && !isErrorState();
}
