#include "sourceio.h"
#include "json/jsonstructureloader.h"

SourceIo::SourceIo(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties) :
    m_ioDevice(ioDevice),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(sourceProperties)),
    m_sourceProperties(sourceProperties)
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceIo::onIoGroupFinished);
}

int SourceIo::startTransaction(IoQueueEntry::Ptr transferGroup)
{
    m_ioQueue.enqueueTransferGroup(transferGroup);
    return transferGroup->getGroupId();
}

IoGroupGenerator SourceIo::getIoGroupGenerator() const
{
    return m_ioGroupGenerator;
}

SourceProperties SourceIo::getProperties() const
{
    return m_sourceProperties;
}

void SourceIo::onIoGroupFinished(IoQueueEntry::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
