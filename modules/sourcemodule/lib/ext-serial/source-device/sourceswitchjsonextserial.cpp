#include "sourceswitchjsonextserial.h"
#include "sourceioextserial.h"

SourceSwitchJsonExtSerial::SourceSwitchJsonExtSerial(AbstractSourceIoPtr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch) :
    m_sourceIo(sourceIo),
    m_ioGroupGenerator(sourceIo->getCapabilities()),
    m_sourceNotificationSwitch(sourceNotificationSwitch),
    m_persistentParamState(PersistentJsonState(sourceIo->getCapabilities())),
    m_loadpointCurrent(m_persistentParamState.loadJsonState()),
    m_loadpointRequestedLast(m_loadpointCurrent)
{
    connect(m_sourceNotificationSwitch.get(), &SourceTransactionStartNotifier::sigTransationStarted,
            this, &SourceSwitchJsonExtSerial::onSwitchTransactionStarted);
    connect(m_sourceIo.get(), &SourceIoExtSerial::sigResponseReceived,
            this, &SourceSwitchJsonExtSerial::onResponseReceived);
}

int SourceSwitchJsonExtSerial::switchState(const JsonParamApi &paramState)
{
    m_loadpointRequestedLast = paramState;
    IoQueueGroup::Ptr transferGroup = m_ioGroupGenerator.generateOnOffGroup(m_loadpointRequestedLast);
    m_sourceNotificationSwitch->startTransactionWithNotify(transferGroup);
    return transferGroup->getGroupId();
}

JsonParamApi SourceSwitchJsonExtSerial::getCurrLoadpoint()
{
    return m_loadpointCurrent;
}

JsonParamApi SourceSwitchJsonExtSerial::getLoadpointRequestedLast()
{
    return m_loadpointRequestedLast;
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
        // This will not work properly for multiple transactions running
        m_loadpointCurrent = m_loadpointRequestedLast;
        m_persistentParamState.saveJsonState(m_loadpointCurrent);
    }
    emit sigSwitchFinished(transferGroup->passedAll(), transferGroup->getGroupId());
}
