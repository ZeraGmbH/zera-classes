#include "sourceioextserial.h"
#include "jsonstructureloader.h"

SourceIoExtSerial::SourceIoExtSerial(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties) :
    m_ioDevice(ioDevice),
    m_capabilities(JsonStructureLoader::loadJsonStructure(sourceProperties))
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceIoExtSerial::onIoGroupFinished);
}

int SourceIoExtSerial::startTransaction(IoQueueGroup::Ptr transferGroup)
{
    m_ioQueue.enqueueTransferGroup(transferGroup);
    return transferGroup->getGroupId();
}

QJsonObject SourceIoExtSerial::getCapabilities() const
{
    return m_capabilities;
}

void SourceIoExtSerial::onIoGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
