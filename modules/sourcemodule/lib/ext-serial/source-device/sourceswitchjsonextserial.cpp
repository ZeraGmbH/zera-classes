#include "sourceswitchjsonextserial.h"
#include "sourceioextserial.h"

SourceSwitchJsonExtSerial::SourceSwitchJsonExtSerial(AbstractSourceIoPtr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch) :
    m_sourceIo(sourceIo),
    m_ioGroupGenerator(sourceIo->getCapabilities()),
    m_sourceNotificationSwitch(sourceNotificationSwitch),
    m_persistentParamState(PersistentJsonState(sourceIo->getCapabilities())),
    m_paramsCurrent(m_persistentParamState.loadJsonState()),
    m_paramsRequested(m_paramsCurrent)
{
    connect(m_sourceNotificationSwitch.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceSwitchJsonExtSerial::onSwitchTransactionStarted);
    connect(m_sourceIo.get(), &SourceIoExtSerial::sigResponseReceived,
            this, &SourceSwitchJsonExtSerial::onResponseReceived);
}

int SourceSwitchJsonExtSerial::switchState(const JsonParamApi &paramState)
{
    m_paramsRequested = paramState;
    IoQueueGroup::Ptr transferGroup = m_ioGroupGenerator.generateOnOffGroup(m_paramsRequested);
    m_sourceNotificationSwitch->startTransactionWithNotify(transferGroup);
    return transferGroup->getGroupId();
}

JsonParamApi SourceSwitchJsonExtSerial::getCurrLoadpoint()
{
    return m_paramsCurrent;
}

JsonParamApi SourceSwitchJsonExtSerial::getRequestedLoadState()
{
    return m_paramsRequested;
}

void SourceSwitchJsonExtSerial::onSwitchTransactionStarted(int dataGroupId)
{
    m_pendingSwitchIds.setPending(dataGroupId);
}

void SourceSwitchJsonExtSerial::onResponseReceived(const IoQueueGroup::Ptr transferGroup)
{
    int groupId = transferGroup->getGroupId();
    if(m_pendingSwitchIds.isPendingAndRemoveIf(groupId))
        handleSwitchResponse(transferGroup);
}

void SourceSwitchJsonExtSerial::handleSwitchResponse(const IoQueueGroup::Ptr transferGroup)
{
    if(transferGroup->passedAll()) {
        m_paramsCurrent = m_paramsRequested;
        m_persistentParamState.saveJsonState(m_paramsCurrent);
    }
    emit sigSwitchFinished(transferGroup->passedAll(), transferGroup->getGroupId());
}
