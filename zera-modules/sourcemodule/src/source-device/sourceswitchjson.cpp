#include "sourceswitchjson.h"

SourceSwitchJson::SourceSwitchJson(ISourceIo *sourceDevice, SourceTransactionStartNotifier *sourceNotificationSwitch) :
    m_sourceDevice(sourceDevice),
    m_sourceNotificationSwitch(sourceNotificationSwitch)
{
    m_persistentParamState = new PersistentJsonState(sourceDevice->getProperties());
    m_paramsCurrent = m_persistentParamState->loadJsonState();
    m_paramsRequested = m_paramsCurrent;

    connect(m_sourceNotificationSwitch, &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceSwitchJson::onSwitchTransactionStarted);
    connect(m_sourceDevice, &SourceIo::sigResponseReceived,
            this, &SourceSwitchJson::onResponseReceived);
}

void SourceSwitchJson::switchState(JsonParamApi paramState)
{
    m_paramsRequested = paramState;
    IoQueueEntry::Ptr transferGroup = m_sourceDevice->getIoGroupGenerator().generateOnOffGroup(m_paramsRequested);
    m_sourceNotificationSwitch->startTransactionWithNotify(transferGroup);
}

void SourceSwitchJson::switchOff()
{
    JsonParamApi paramOff = m_paramsCurrent;
    paramOff.setOn(false);
    switchState(paramOff);
}

JsonParamApi SourceSwitchJson::getCurrLoadState()
{
    return m_paramsCurrent;
}

void SourceSwitchJson::onSwitchTransactionStarted(int dataGroupId)
{
    m_pendingSwitchIds.setPending(dataGroupId);
}

void SourceSwitchJson::onResponseReceived(const IoQueueEntry::Ptr transferGroup)
{
    int groupId = transferGroup->getGroupId();
    if(m_pendingSwitchIds.isPendingAndRemoveIf(groupId)) {
        handleSwitchResponse(transferGroup);
    }
}

void SourceSwitchJson::handleSwitchResponse(const IoQueueEntry::Ptr transferGroup)
{
    if(transferGroup->passedAll()) {
        m_paramsCurrent = m_paramsRequested;
        m_persistentParamState->saveJsonState(m_paramsCurrent);
    }
    emit sigSwitchFinished();
}
