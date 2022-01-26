#include "sourcestatecontroller.h"

SourceStateController::SourceStateController(ISourceDevice *sourceDevice,
                                               SourceTransactionStartNotifier *sourceNotificationSwitch,
                                               SourceTransactionStartNotifier *sourceNotificationStateQuery) :
    m_sourceDevice(sourceDevice),
    m_sourceNotificationSwitch(sourceNotificationSwitch),
    m_sourceNotificationStateQuery(sourceNotificationStateQuery)

{
    m_pollTimer.setSingleShot(false);
    m_pollTimer.setInterval(500);
    enablePolling();

    connect(&m_pollTimer, &QTimer::timeout,
            this, &SourceStateController::onPollTimer);

    connect(m_sourceNotificationSwitch, &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onSwitchTransactionStarted);
    connect(m_sourceNotificationStateQuery, &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceStateController::onStateQueryTransationStarted);

    connect(m_sourceDevice, &SourceDevice::sigResponseReceived,
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
        IoQueueEntry::Ptr transferGroup = m_sourceDevice->getIoGroupGenerator().generateStatusPollGroup();
        m_sourceNotificationStateQuery->startTransactionWithNotify(transferGroup);
        bStarted = true;
    }
    return bStarted;
}

int SourceStateController::isPeriodicPollActive()
{
    return m_pollTimer.isActive();
}

void SourceStateController::onPollTimer()
{
    tryStartPollNow();
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

void SourceStateController::setState(SourceStates state)
{
    if(m_currState.isUnequal(state)) {
        m_currState.setState(state);
        emit sigStateChanged(state);
        setPollingOnStateChange();
    }
}

void SourceStateController::setPollingOnStateChange()
{
    if(!m_currState.isError()) {
        enablePolling();
    }
    else {
        disablePolling();
    }
}

void SourceStateController::handleSwitchResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(!transferGroup->passedAll() && !m_currState.isError()) {
        setState(SourceStates::ERROR_SWITCH);
    }
    else if(m_currState.isSwitchBusy()) {
        setState(SourceStates::IDLE);
    }
}

void SourceStateController::handleStateResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(!transferGroup->passedAll() && !m_currState.isError()) {
        setState(SourceStates::ERROR_POLL);
    }
    else if(m_currState.isUndefined()) {
        setState(SourceStates::IDLE);
    }
}
