#include "sourceswitchjson.h"
#include "sourceioextserial.h"

SourceSwitchJson::SourceSwitchJson(AbstractSourceIoPtr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch) :
    m_sourceIo(sourceIo),
    m_ioGroupGenerator(sourceIo->getCapabilities()),
    m_sourceNotificationSwitch(sourceNotificationSwitch)
{
    m_persistentParamState = new PersistentJsonState(sourceIo->getCapabilities());
    m_paramsCurrent = m_persistentParamState->loadJsonState();
    m_paramsRequested = m_paramsCurrent;

    connect(m_sourceNotificationSwitch.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceSwitchJson::onSwitchTransactionStarted);
    connect(m_sourceIo.get(), &SourceIoExtSerial::sigResponseReceived,
            this, &SourceSwitchJson::onResponseReceived);
}

void SourceSwitchJson::switchState(JsonParamApi paramState)
{
    m_paramsRequested = paramState;
    IoQueueGroup::Ptr transferGroup = m_ioGroupGenerator.generateOnOffGroup(m_paramsRequested);
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

JsonParamApi SourceSwitchJson::getRequestedLoadState()
{
    return m_paramsRequested;
}

void SourceSwitchJson::onSwitchTransactionStarted(int dataGroupId)
{
    m_pendingSwitchIds.setPending(dataGroupId);
}

void SourceSwitchJson::onResponseReceived(const IoQueueGroup::Ptr transferGroup)
{
    int groupId = transferGroup->getGroupId();
    if(m_pendingSwitchIds.isPendingAndRemoveIf(groupId)) {
        handleSwitchResponse(transferGroup);
    }
}

void SourceSwitchJson::handleSwitchResponse(const IoQueueGroup::Ptr transferGroup)
{
    if(transferGroup->passedAll()) {
        m_paramsCurrent = m_paramsRequested;
        m_persistentParamState->saveJsonState(m_paramsCurrent);
    }
    emit sigSwitchFinished();
}
