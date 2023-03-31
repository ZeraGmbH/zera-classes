#include "sourceio.h"
#include "jsonstructureloader.h"

SourceIo::SourceIo(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties) :
    m_ioDevice(ioDevice),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(sourceProperties)),
    m_sourceProperties(sourceProperties)
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceIo::onIoGroupFinished);
}

int SourceIo::startTransaction(IoQueueGroup::Ptr transferGroup)
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

void SourceIo::onIoGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
